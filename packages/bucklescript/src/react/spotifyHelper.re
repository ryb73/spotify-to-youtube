open Js.Promise;

let clientId = "35988437b3404ab08d67eaced43c4997";

/* I'm going to be lazy and just cache this globally */
let myUserId : ref (option string) = ref None;

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

let loadMyUserId spotify => {
    spotify##getMe ()
        |> then_ (fun data => {
            myUserId := Some data##body##id;
            resolve "";/*data##body##id;*/
        });
};

let getMyUserId spotify =>
    switch !myUserId {
        | None => loadMyUserId spotify
        | Some userId => resolve userId
    };

let getMyPlaylists spotify => {
    getMyUserId spotify
        |> then_ (fun userId => spotify##getUserPlaylists userId);
};