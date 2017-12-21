open Webapi.Dom.Window;
open Webapi.Dom.Location;

type state = {
  selectedPlaylist: option(Js.t(Spotify.playlist)),
  ytHelper: option(YouTubeHelper.t),
  exportFinished: bool
};

type action =
  | SetSelectedPlaylist(Js.t(Spotify.playlist))
  | SetYtHelper(YouTubeHelper.t)
  | SetExportFinished;

let component = ReasonReact.reducerComponent("Page");
let make = (_) => {
  let parsedHashString = {
    let hashStr = Webapi.Dom.window |> location |> hash;
    let queryString =
      switch (String.length(hashStr)) {
        | 0 => ""
        | _ => String.sub(hashStr, 1, String.length(hashStr) - 1)
      };
    module Parser =
      Qs.MakeParser(
        {
          class type t = {
            pub access_token: Js.undefined(string);
            pub token_type: string;
            pub expires_in: int;
            pub state: string
          };
        }
      );
    Parser.parse(queryString)
  };

  let spotify = SpotifyHelper.create();

  let afterExport = (_) => SetExportFinished;

  let validateSpotifyResponse = ({ ReasonReact.reduce }, access_token) =>
    switch parsedHashString##state {
      | "state" =>
        spotify##setAccessToken(access_token);
        <PlaylistList spotify playlistSelected=(reduce((playlist) => SetSelectedPlaylist(playlist))) />
      | _ => <span> (ReasonReact.stringToElement("There was an error connecting to Spotify.")) </span>
    };

  {
    ...component,

    initialState: () => {selectedPlaylist: None, ytHelper: None, exportFinished: false},

    render: (bag) => {
      let { ReasonReact.state, reduce } = bag;
      switch state.selectedPlaylist {
        | None =>
          switch (Js.Undefined.to_opt(parsedHashString##access_token)) {
          | None => <PromptConnectSpotify />
          | Some(access_token) => validateSpotifyResponse(bag, access_token)
          }
        | Some(selectedPlaylist) =>
          switch state.ytHelper {
            | None => <PromptConnectYouTube onSignedIn=(reduce((ytHelper) => SetYtHelper(ytHelper))) />
            | Some(ytHelper) => {
              ! state.exportFinished ?
                <AllowExportPlaylist
                  spotify
                  ytHelper
                  playlistId=selectedPlaylist##id
                  playlistOwnerId=selectedPlaylist##owner##id
                  onNextStep=(reduce(afterExport))
                /> :
                <ImportPlaylist playlistName=selectedPlaylist##name />
            }
          }
      }
    },

    reducer: (action, state) => {
      switch action {
        | SetSelectedPlaylist(playlist) => ReasonReact.Update({...state, selectedPlaylist: Some(playlist)})
        | SetYtHelper(ytHelper) => ReasonReact.Update({...state, ytHelper: Some(ytHelper)})
        | SetExportFinished => ReasonReact.Update({...state, exportFinished: true})
      };
    }
  };
};
