open Qs;

module ValidateSpotifyCode = {
    include ReactRe.Component;

    module QueryString = {
        class type t = {
            pub code : Js.undefined string;
            pub state : string;
        };
    };

    module MyQs = MakeParser(QueryString);

    type props = {
        code: string,
        parsedQueryString: Js.t QueryString.t
    };

    let name = "ValidateSpotifyCode";

    let render { props } =>
        <div>
            (ReactRe.stringToElement ("Code: " ^ props.code))<br />
            (ReactRe.stringToElement ("State: " ^ props.parsedQueryString##state))<br />
        </div>;
};

include ReactRe.CreateComponent ValidateSpotifyCode;

let createElement ::parsedQueryString ::code => wrapProps { parsedQueryString, code };

let parseQueryString = ValidateSpotifyCode.MyQs.parse;