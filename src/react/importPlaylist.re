open Js.Promise;

let s2e = ReasonReact.stringToElement;

type props = {playlistName: string};
type status =
  | Ready
  | NamingConflict(string, list(array(string))) /* existingPlaylistId * csvData */
  | Processing(string) /* message */
  | Done;

type state = {
  errors: array(string),
  status
};

type action =
  | AddError(string)
  | SetStatus(status);

let go = (reduce, action) =>  reduce((_) => action, ());

let artistColumn = 0;
let songColumn = 1;
let videoUrlColumn = 4;
let validateCsv = (csv) => {
  let csvList = Array.to_list(csv);
  switch csvList {
  | [headers, ...data] => headers[videoUrlColumn] === "Video URL" ? Some(data) : None
  | [] => None
  }
};

let setErrorMessage = ({ ReasonReact.reduce }, errorMessage) =>
  go(reduce, AddError(errorMessage));

let getVideoUrlFromRow = (row) => row[videoUrlColumn];

let getVideoIdFromUrl = (url) => {
  let pieces = Js.String.split("https://youtu.be/", url);
  /* Allow for blank or "not found" rows */
  if (Js.Array.length(pieces) < 2) {
    None
  } else {
    Some(pieces[1])
  }
};

let getVideoNameFromRow = (row) => {
  let artist = row[artistColumn];
  let song = row[songColumn];
  {j|$artist – $song|j}
};

let addVideoToPlaylist = (bag, playlistId, (videoId, videoName)) => {
  Js.log2("adding video", videoId);
  go(bag.ReasonReact.reduce, SetStatus(Processing("Adding " ++ videoName)));

  switch videoId {
  | None => resolve()
  | Some(id) =>
    YouTubeHelper.insertPlaylistItem(playlistId, id)
    |> then_((_) => resolve())
    |> catch(
          (err) => {
            Js.log(err);
            setErrorMessage(bag, "Error adding video " ++ videoName ++ " to playlist.");
            resolve()
          }
        )
  }
};

let parseVideoRow = (row) => (
  row |> getVideoUrlFromRow |> getVideoIdFromUrl,
  row |> getVideoNameFromRow
);

let addSongsToPlaylist = (bag, playlistId, data) => {
  data
    |> List.map(parseVideoRow)
    |> List.fold_left((promise, videoInfo) => promise |> then_((_) => addVideoToPlaylist(bag, playlistId, videoInfo)), resolve())
    |> then_((_) => {
      go(bag.reduce, SetStatus(Done));
      resolve()
    });
};

let findExistingPlaylistWithName = (name, existingPlaylists) => {
  let matchingPlaylist =
    existingPlaylists##result##items |> Js.Array.find((item) => item##snippet##title === name);
  switch matchingPlaylist {
  | Some(playlist) => Some(playlist##id)
  | None => None
  }
};

let genericErrorHandler = (bag, err) => {
  Js.log(err);
  setErrorMessage(bag, "There was an unknown error.");
  resolve()
};

let beginPlaylistCreation = (playlistName, bag, (csvData, playlists)) => {
  switch (validateCsv(csvData)) {
    | Some(validatedData) =>
      switch (findExistingPlaylistWithName(playlistName, playlists)) {
        | Some(playlistId) =>
          go(bag.ReasonReact.reduce, SetStatus(NamingConflict(playlistId, validatedData)));
          resolve()
        | None =>
          go(bag.reduce, SetStatus(Processing("Creating playlist")));
          YouTubeHelper.createPlaylist(playlistName)
            |> then_ @@ ((data) => addSongsToPlaylist(bag, data##result##id, validatedData))
            |> catch @@ genericErrorHandler(bag)
      }
    | None =>
      setErrorMessage(bag, "The file you've selected appears to have an invalid format.");
      resolve()
  }
};

let chooseCsv = (playlistName, _, bag) => {
  let inputFilename =
    Electron.Electron.remote##dialog##showOpenDialog(Js.undefined, Js.undefined);
  let readStream = NodeFs.FileReadStream.create(inputFilename[0], Js.undefined);
  let csvStream = FastCsv.parse();
  NodeFs.FileReadStream.pipe(readStream, FastCsv.Parse.writeable(csvStream));
  let pCsv = StreamToArray.toArray @@ FastCsv.Parse.readable(csvStream);
  let pPlaylists = YouTubeHelper.getUserPlaylists();

  Js.Promise.all2((pCsv, pPlaylists))
    |> then_ @@ beginPlaylistCreation(playlistName, bag)
    |> catch @@ genericErrorHandler(bag);

  ();
};

let generatePlaylistName = (baseName) => {
  let date = Js.Date.make();
  let year = Js.Date.getFullYear(date);
  let month = Js.Date.getMonth(date) +. 1.0;
  let day = Js.Date.getDate(date);
  let hour = Js.Date.getHours(date);
  let minute = Js.Date.getMinutes(date);
  let second = Js.Date.getSeconds(date);
  {j|$baseName $year-$month-$day $hour:$minute:$second|j}
};

let useDifferentPlaylistName = (playlistName, csvData, _, bag) => {
  let { ReasonReact.reduce } = bag;
  let newName = generatePlaylistName(playlistName);

  YouTubeHelper.createPlaylist(newName)
    |> then_ @@ ((data) => addSongsToPlaylist(bag, data##result##id, csvData))
    |> catch @@ genericErrorHandler(bag);

  go(reduce, SetStatus(Processing("Creating playlist")));
};

let useExistingPlaylist = (playlistId, csvData, _, bag) => {
  addSongsToPlaylist(bag, playlistId, csvData) |> catch @@ genericErrorHandler(bag);
  ();
};

let renderNameConflictPrompt = (playlistName, { ReasonReact.handle }, playlistId, csvData) =>
  <p>
    (s2e @@ "A YouTube playlist with the name \"" ++ playlistName ++ "\" already exists. What would you like to do? ")
    <a href="#" onClick=(handle(useDifferentPlaylistName(playlistName, csvData)))>
      (s2e("Use a different name for the playlist"))
    </a>
    (s2e(" "))
    <a href="#" onClick=(handle(useExistingPlaylist(playlistId, csvData)))>
      (s2e("Add to the existing playlist"))
    </a>
  </p>;

let renderErrors = (errors) => {
  let renderedErrors = errors
    |> Js.Array.mapi((message, i) =>
      <div key=(Js.String.make(i))> (s2e(message)) </div>
    );
  <div> (ReasonReact.arrayToElement(renderedErrors)) </div>
};

let renderChooseLink = (playlistName, { ReasonReact.handle }) =>
  <a href="#" onClick=(handle(chooseCsv(playlistName)))> (s2e("Choose CSV file")) </a>;

let renderProcessingMessage = (message) =>
  <p> <i className="fa fa-refresh fa-spin" /> (s2e @@ " " ++ message) </p>;

let renderFinishedMessage = ({ ReasonReact.state }) => {
  let message =
      Js.Array.length(state.errors) > 0 ?
        " Done, but there were some errors. Some songs might be missing from your playlist." :
        " Done";
  <p> <i className="fa fa-check" /> (s2e(message)) </p>
};

let renderStatusSection = (playlistName, bag) => {
  let { ReasonReact.state } = bag;
  let body =
    switch state.status {
    | Ready => renderChooseLink(playlistName, bag)
    | NamingConflict(playlistId, csvData) => renderNameConflictPrompt(playlistName, bag, playlistId, csvData)
    | Processing(message) => renderProcessingMessage(message)
    | Done => renderFinishedMessage(bag)
    };
  <div> body (renderErrors(state.errors)) </div>
};

let make = (~playlistName, _) => {
  ...(ReasonReact.reducerComponent("ImportPlaylist")),

  initialState: () => {errors: [||], status: Ready},

  render: (bag) =>
    <div>
      <h1> (s2e("Step 5: Import Playlist CSV")) </h1>
      <p>
        (
          s2e @@
          "The CSV you exported in the previous step will now be imported and the videos specified "
          ++ "in the file will be added to a new YouTube playlist."
        )
      </p>
      (renderStatusSection(playlistName, bag))
    </div>,

  reducer: (action, state) => {
    switch action {
      | AddError(msg) => ReasonReact.Update({ ...state, errors: Js.Array.concat([|msg|], state.errors) })
      | SetStatus(status) => ReasonReact.Update({ ...state, status })
    };
  }
};
