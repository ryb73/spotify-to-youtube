open ReasonJs;
open Dom.Window;
open Dom.Location;

module Page = {
    include ReactRe.Component;

    type props = unit;
    let name = "Page";

    let searchStr = Dom.window |> location |> search;
    let queryString = switch(String.length searchStr) {
        | 0 => ""
        | _ => String.sub searchStr 1 (String.length searchStr - 1)
    };
    Js.log ("!! " ^ queryString);

    let parsedQueryString = ValidateSpotifyResponse.parseQueryString queryString;

    let render _ => switch (Js.Undefined.to_opt parsedQueryString##code) {
        | None => <PromptConnectSpotify />;
        | Some code => <ValidateSpotifyResponse code parsedQueryString />;
    };
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();