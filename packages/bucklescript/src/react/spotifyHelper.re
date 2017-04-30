let clientId = "35988437b3404ab08d67eaced43c4997";

let create () => Spotify.create [%bs.obj {
    clientId: clientId,
    clientSecret: Js.Undefined.empty,
    redirectUri: "http://localhost:54380/"
}];

let authUrl scope state => {
    let queryString = Qs.stringify [%bs.obj {
        client_id: clientId,
        response_type: "token",
        redirect_uri: "http://localhost:54380/",
        state: state,
        scope: String.concat " " scope
    }];
    "https://accounts.spotify.com/authorize?" ^ queryString;
};