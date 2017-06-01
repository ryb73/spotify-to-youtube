open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component.Stateful;
    let name = "Page";
    type props = unit;
    type state = {
        selectedPlaylist : option (Js.t Spotify.playlist),
        ytHelper: option YouTubeHelper.t,
        exportFinished: bool
    };

    let getInitialState _ => {
        selectedPlaylist: None,
        ytHelper: None,
        exportFinished: false
     };

    let parsedHashString = {
        let hashStr = Dom.window |> location |> hash;
        let queryString = switch(String.length hashStr) {
            | 0 => ""
            | _ => String.sub hashStr 1 (String.length hashStr - 1)
        };

        module Parser = Qs.MakeParser({
            class type t = {
                pub access_token : Js.undefined string;
                pub token_type : string;
                pub expires_in : int;
                pub state : string;
            };
        });

        Parser.parse queryString;
    };

    let spotify = SpotifyHelper.create ();

    let playlistSelected { state } playlist => {
        Some { ...state, selectedPlaylist: Some playlist };
    };

    let onSignedIntoYouTube { state } ytHelper => {
        Some { ...state, ytHelper: Some ytHelper };
    };

    let afterExport { state } _ => {
        Some { ...state, exportFinished: true };
    };

    let validateSpotifyResponse { updater } access_token => {
        switch (parsedHashString##state) {
            | "state" => {
                spotify##setAccessToken access_token;
                <PlaylistList spotify playlistSelected=(updater playlistSelected) />;
            };

            | _ =>
                <span>(ReactRe.stringToElement "There was an error connecting to Spotify.")</span>;
        };
    };

    let render bag => {
        let { state, updater } = bag;

        switch state.selectedPlaylist {
            | None => switch (Js.Undefined.to_opt parsedHashString##access_token) {
                | None => <PromptConnectSpotify />;
                | Some access_token => validateSpotifyResponse bag access_token;
            };

            | Some selectedPlaylist => switch state.ytHelper {
                | None =>
                    <PromptConnectYouTube onSignedIn=(updater onSignedIntoYouTube) />

                | Some ytHelper =>
                    (not state.exportFinished) ?
                        <AllowExportPlaylist spotify ytHelper playlistId=selectedPlaylist##id
                            onNextStep=(updater afterExport) />
                    :
                        <ImportPlaylist playlistName=selectedPlaylist##name />
            };
        };
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();