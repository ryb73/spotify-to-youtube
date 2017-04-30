module PromptConnectSpotify = {
    include ReactRe.Component;

    type props = unit;
    let name = "PromptConnectSpotify";
    let spotifyUrl = {
        let spotify = Spotify.create [%bs.obj {
            clientId: "35988437b3404ab08d67eaced43c4997",
            clientSecret: Js.Undefined.empty,
            redirectUri: "http://localhost:54380/"
        }];

        let requiredPermissions = [| "playlist-read-private", "playlist-read-collaborative" |];
        spotify##createAuthorizeURL requiredPermissions "state";
    };

    let render _ =>
        <div>
            <h1>(ReactRe.stringToElement "Step 1: Log into Spotify")</h1>
            <p>
                <a href=spotifyUrl>(ReactRe.stringToElement "Log In")</a>
            </p>
        </div>;
};

include ReactRe.CreateComponent PromptConnectSpotify;

let createElement = wrapProps ();