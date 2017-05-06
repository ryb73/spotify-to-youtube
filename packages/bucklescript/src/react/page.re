open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component.Stateful;
    let name = "Page";
    type props = unit;
    type state = {
        selectedPlaylistId : option string,
        signedIntoYouTube: bool
    };

    let getInitialState _ => {
        selectedPlaylistId: None,
        signedIntoYouTube: false
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

    let playlistSelected { state } playlistId => {
        Some { ...state, selectedPlaylistId: Some playlistId };
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

    let onSignedIntoYouTube { state } _ => {
        Some { ...state, signedIntoYouTube: true };
    };

    let render bag => {
        let { state, updater } = bag;

        switch state.selectedPlaylistId {
            | None => switch (Js.Undefined.to_opt parsedHashString##access_token) {
                | None => <PromptConnectSpotify />;
                | Some access_token => validateSpotifyResponse bag access_token;
            };

            | Some selectedPlaylistId => switch state.signedIntoYouTube {
                | true => <AllowExportPlaylist spotify playlistId=selectedPlaylistId />;
                | false => <PromptConnectYouTube onSignedIn=(updater onSignedIntoYouTube) />;
            };
        };
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();