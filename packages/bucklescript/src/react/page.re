open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component;
    let name = "Page";
    type props = unit;

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

    let validateSpotifyResponse access_token => {
        switch (parsedHashString##state) {
            | "state" => {
                spotify##setAccessToken access_token;
                <PlaylistList spotify />;
            };

            | _ =>
                <span>(ReactRe.stringToElement "There was an error connecting to Spotify.")</span>;
        };
    };

    let render _ => {
        switch (Js.Undefined.to_opt parsedHashString##access_token) {
            | None => <PromptConnectSpotify />;
            | Some access_token => validateSpotifyResponse access_token;
        };
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();