open Js.Promise;

module CsvFormatter =
  FastCsv.Format(
    {
      type t = Js.t(VideoMatcher.csvRow);
    }
  );

let s2e = ReasonReact.stringToElement;

type props = {
  spotify: Js.t(Spotify.t),
  playlistId: string,
  onNextStep: unit => unit,
  ytHelper: YouTubeHelper.t
};

type state = {
  statusMessage: option(string),
  errorMessage: option(string),
  matchingFinished: bool
};

type action =
  | SetStatusMessage(string)
  | SetErrorMessage(string)
  | MatchingFinished;

let go = (reduce, action) => reduce(() => action, ());

let component = ReasonReact.reducerComponent("AllowExportPlaylist");

let make = (~spotify, ~playlistId, ~onNextStep, ~ytHelper, ~playlistOwnerId, _) => {
  let getTracks = () => SpotifyHelper.getPlaylistTracks(spotify, playlistId, playlistOwnerId);

  let writeMatch = (csvStream, formattedMatch) => {
    CsvFormatter.write(csvStream, formattedMatch);
    resolve()
  };

  let updateStatusForTrack = ({ ReasonReact.reduce }, track, currentIndex, numRemaining) => {
    let artist = (track##track##artists)[0]##name;
    let song = track##track##name;
    let currentTrack = currentIndex + 1;
    let totalTracks = currentTrack + numRemaining;
    let message = {j|[ $currentTrack / $totalTracks ] $artist – $song|j};

    go(reduce, SetStatusMessage(message));
  };

  /* Search for (and write) a match for each track in series */
  let rec matchEachTrack = (bag, csvStream, tracks) => {
    Js.log("matching");
    switch tracks {
      | [] => resolve()
      | [(track, index), ...remaining] =>
        updateStatusForTrack(bag, track, index, List.length(tracks));
        VideoMatcher.matchTrack(ytHelper, track)
          |> then_ @@ writeMatch(csvStream)
          |> then_((_) => matchEachTrack(bag, csvStream, remaining))
    }
  };

  let openCsvFile = (bag, filename) => {
    let headers = {
      "artist": "Artist",
      "song": "Song",
      "matchType": "Match Category",
      "videoTitle": "Video Title",
      "videoUrl": "Video URL",
      "searchUrl": "Search URL"
    };
    NodeFs.FileWriteStream.create(filename, Js.undefined, (fileStream) =>
      CsvFormatter.create(Js.undefined, (csvStream) => {
        CsvFormatter.pipe(csvStream, NodeFs.FileWriteStream.writeable(fileStream));
        CsvFormatter.write(csvStream, headers);
        Js.log("getting tracks");
        getTracks()
          |> then_((trackArr) => {
            Js.log("got tracks");
            trackArr
              |> Js.Array.mapi((track, i) => (track, i))
              |> Array.to_list
              |> resolve
          })
          |> then_ @@ matchEachTrack(bag, csvStream);
      })
    )
  };

  let beginMatching = (_, bag) => {
    let { ReasonReact.reduce } = bag;
    let outputFilename = Electron.Electron.remote##dialog##showSaveDialog(Js.undefined, Js.undefined);
    openCsvFile(bag, outputFilename)
      |> then_((_) => {
        go(reduce, MatchingFinished);
        resolve();
      })
      |> catch((err) => {
        Js.log(err);
        let message = "An error occurred. Please make sure you have the ability to save to the location you chose.";
        go(reduce, SetErrorMessage(message));
        resolve();
      });

    go(reduce, SetStatusMessage("Loading playlist tracks"));
  };

  let goToNextStep = (_) => onNextStep();

  let renderOptions = ({ ReasonReact.handle }) =>
    <p>
      <a href="#" onClick=(handle(beginMatching))> (s2e("Begin matching songs to videos")) </a>
      (s2e(" | "))
      <a href="#" onClick=(goToNextStep)>
        (s2e({js|I already have a CSV – Skip this step|js}))
      </a>
    </p>;

  let renderErrorMessage = ({ ReasonReact.state }) =>
    switch state.errorMessage {
      | None => s2e("")
      | Some(msg) => s2e(msg)
    };

  let renderStatusSection = (bag) => {
    let { ReasonReact.state } = bag;
    if (state.matchingFinished) {
      <span>
        (s2e("Complete! "))
        <a href="#" onClick=(goToNextStep)> (s2e("Click here to continue.")) </a>
      </span>
    } else {
      switch state.statusMessage {
        | Some(msg) => <div> <i className="fa fa-refresh fa-spin" /> (s2e @@ " " ++ msg) </div>
        | None => <div> (renderOptions(bag)) (renderErrorMessage(bag)) </div>
      }
    }
  };

  {
    ...component,

    initialState: () => {statusMessage: None, errorMessage: None, matchingFinished: false},

    render: (bag) =>
      <div>
        <h1> (s2e("Step 4: Match Songs to YouTube Videos")) </h1>
        <p>
          (
            s2e @@
            "A CSV file, which can be opened using Microsoft Excel or other similar "
            ++ "applications, will now be created. The file will include each song from your Spotify playlist "
            ++ "along with the best match on YouTube. You can modify the CSV to add/remove/modify any songs "
            ++ "you wish. The CSV will then be used in the next step to create the YouTube playlist."
          )
        </p>
        <p>
          (
            s2e @@
            "If you've already completed this step and have a CSV ready, you can "
            ++ "skip this and move on to the next step."
          )
        </p>
        (renderStatusSection(bag))
      </div>,

    reducer: (action, state) => {
      switch action {
        | SetStatusMessage(msg) => ReasonReact.Update { ...state, statusMessage: Some(msg) }
        | MatchingFinished => ReasonReact.Update { ...state, matchingFinished: true, statusMessage: None }
        | SetErrorMessage(msg) => ReasonReact.Update { ...state, errorMessage: Some(msg), statusMessage: None }
      };
    }
  };
};
