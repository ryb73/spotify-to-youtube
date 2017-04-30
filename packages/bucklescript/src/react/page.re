open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component.Stateful;
    let name = "Page";
    type props = unit;

    type spotifyAuthStatus = [ `Unauthed | `Authed | `Error ];

    type state = { spotifyAuthed: spotifyAuthStatus };

    let getInitialState _ => { spotifyAuthed: `Unauthed };

    let parsedQueryString = {
        let searchStr = Dom.window |> location |> search;
        let queryString = switch(String.length searchStr) {
            | 0 => ""
            | _ => String.sub searchStr 1 (String.length searchStr - 1)
        };

        ValidateSpotifyResponse.parseQueryString queryString;
    };

    let spotify = SpotifyHelper.create ();

    let setSpotifyTokens { setState } data => {
        spotify##setAccessToken data##access_token;

        setState (fun { state } => { ...state, spotifyAuthed: `Authed });

        Js.Promise.resolve ();
    };

    let validateSpotifyResponse bag code => {
        switch (parsedQueryString##state) {
            | "state" => {
                spotify##authorizationCodeGrant code
                    |> Js.Promise.then_ (setSpotifyTokens bag);

                <LoadingScreen message="Connecting to Spotify" />;
            };

            | _ =>
                <span>(ReactRe.stringToElement "There was an error connecting to Spotify.")</span>;
        };
    };

    let render bag => {
        let { state } = bag;

        switch (state.spotifyAuthed) {
            | `Authed => <PromptConnectYoutube />;

            | `Unauthed =>
                switch (Js.Undefined.to_opt parsedQueryString##code) {
                    | None => <PromptConnectSpotify spotify=spotify />;
                    | Some code => validateSpotifyResponse bag code;
                };

            | `Error => <span>(ReactRe.stringToElement "An error has occurred.")</span>;
        };
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();