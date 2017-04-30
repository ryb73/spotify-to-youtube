module PromptConnectSpotify = {
    include ReactRe.Component;
    let name = "PromptConnectSpotify";
    type props = { spotify: Js.t Spotify.t };

    let render { props } => {
        let spotifyUrl = {
            let requiredPermissions = [ "playlist-read-private", "playlist-read-collaborative" ];
            SpotifyHelper.authUrl requiredPermissions "state";
        };

        <div>
            <h1>(ReactRe.stringToElement "Step 1: Log into Spotify")</h1>
            <p>
                <a href=spotifyUrl>(ReactRe.stringToElement "Log In")</a>
            </p>
        </div>;
    };
};

include ReactRe.CreateComponent PromptConnectSpotify;

let createElement ::spotify => wrapProps { spotify: spotify };