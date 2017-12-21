open Js.Promise;

let s2e = ReasonReact.stringToElement;

type state = {
  playlists: option(array(Js.t(Spotify.playlist))),
  errorMessage: option(string)
};

type action =
  | SetPlaylists(option(array(Js.t(Spotify.playlist))))
  | SetErrorMessage(option(string));

let go = (reduce, action) => reduce((_) => action, ());

let setPlaylists = ({ ReasonReact.reduce }, items) => {
  go(reduce, SetPlaylists(Some(items)));
  resolve()
};

let setError = ({ ReasonReact.reduce }, errorMessage) =>
  go(reduce, SetErrorMessage(Some(errorMessage)));

let onPlaylistSelected = (playlistSelected, playlist, e) => {
  ReactEventRe.Mouse.preventDefault(e);
  playlistSelected(playlist);
};

let renderPlaylist = (playlistSelected, playlist) =>
  <li key=playlist##id>
    <a href="#" onClick=(onPlaylistSelected(playlistSelected, playlist))>
      (s2e(playlist##name))
    </a>
  </li>;

let renderPlaylists = (playlistSelected, playlists) => {
  let listItems = playlists |> Js.Array.map(renderPlaylist(playlistSelected));
  <div>
    <p> (s2e("Select a playlist below:")) </p>
    <ul> (ReasonReact.arrayToElement(listItems)) </ul>
  </div>
};

let renderBody = (playlistSelected, { ReasonReact.state }) => {
  switch state.errorMessage {
  | Some(msg) => <span> (s2e("Error: " ++ msg)) </span>
  | None =>
    switch state.playlists {
    | None => <LoadingScreen message="Connecting to Spotify" />
    | Some(playlists) => renderPlaylists(playlistSelected, playlists)
    }
  }
};

let component = ReasonReact.reducerComponent("PlaylistList");

let make = (~spotify, ~playlistSelected, _) => {
  ...component,

  initialState: () => {playlists: None, errorMessage: None},

  didMount: (bag) => {
    SpotifyHelper.getMyPlaylists(spotify)
      |> then_(setPlaylists(bag))
      |> catch((err) => {
        resolve(err); /* prevent from being cleaned up */
        setError(bag, [%bs.raw "err.message"]);
        resolve()
      });

    ReasonReact.NoUpdate;
  },

  render: (bag) =>
    <div>
      <h1>
        (s2e("Step 2: Select a playlist to convert"))
      </h1>
      (renderBody(playlistSelected, bag))
    </div>,

  reducer: (action, state) => {
    switch action {
      | SetPlaylists(playlists) => ReasonReact.Update({ ...state, playlists: playlists })
      | SetErrorMessage(msg) => ReasonReact.Update({ ...state, errorMessage: msg })
    };
  }
};