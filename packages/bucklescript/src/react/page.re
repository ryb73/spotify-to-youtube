external encodeUriComponent : string => string = "encodeURIComponent" [@@bs.val];

module Page = {
    include ReactRe.Component;

    type props = unit;
    let name = "Page";

    let spotifyUrl = {
        let clientId = "35988437b3404ab08d67eaced43c4997";
        let callbackUrl = encodeUriComponent "http://localhost:54380/";
        let requiredPermissions = encodeUriComponent @@ String.concat " " [ "playlist-read-private", "playlist-read-collaborative" ];

        "https://accounts.spotify.com/authorize/?client_id=" ^ clientId ^
            "&response_type=code&redirect_uri=" ^ callbackUrl ^
            "&scope=" ^ requiredPermissions ^ "&state=34fFs29kd09";
    };

    let render _ =>
        <div>
            <h1>(ReactRe.stringToElement "Step 1: Log into Spotify")</h1>
            <p>
                <a href=spotifyUrl>(ReactRe.stringToElement "Log In")</a>
            </p>
        </div>;
};

include ReactRe.CreateComponent Page;

let createElement = wrapProps ();