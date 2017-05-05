open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component.Stateful;
    let name = "Page";
    type props = unit;
    type state = { selectedPlaylistId : option string };

    let getInitialState _ => { selectedPlaylistId: None };

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

    let playlistSelected _ playlistId => {
        Some { selectedPlaylistId: Some playlistId };
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

    let onSignedIntoYouTube _ _ => None;

    let render bag => {
        let { state, updater } = bag;

        switch state.selectedPlaylistId {
            | Some _ => <PromptConnectYouTube onSignedIn=(updater onSignedIntoYouTube) />;
            | None => switch (Js.Undefined.to_opt parsedHashString##access_token) {
                | None => <PromptConnectSpotify />;
                | Some access_token => validateSpotifyResponse bag access_token;
            };
        };
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();