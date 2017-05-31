open Js.Promise;
open Google.YouTube;

external setTimeout : (unit => unit) => int => unit = "" [@@bs.val];

let clientId = "655237426898-8ge9gblffi9184ng84lqhm1il5p5ifaq.apps.googleusercontent.com";

let init () => Google.Client.init clientId [| `YouTubeSSL |];

let listenSignInChange listener => {
    let auth = Google.Auth2.getAuthInstance ();
    auth##isSignedIn##listen listener;
};

let isSignedIn () => {
    let auth = Google.Auth2.getAuthInstance ();
    auth##isSignedIn##get ();
};

let signIn () => {
    let auth = Google.Auth2.getAuthInstance ();
    auth##signIn ();
};

let queryYoutube query => {
    let opts = [%bs.obj {
        part: "snippet",
        q: query,
        maxResults: Js.Undefined.return 20
    }];

    Search.list opts
        |> then_ (fun data => {
            RemoteYouTubeCache.set query data;
            resolve data;
        });
};

/* The UI thread won't get a chance to update without this */
let promisify v => {
    Js.Promise.make (fun ::resolve reject::_ => {
        let timeoutFun _ => {
            resolve v [@bs];
        };

        setTimeout timeoutFun 1;
    });
};

let doSearch query => {
    let cachedValue = RemoteYouTubeCache.get query;
    switch cachedValue {
        | None => queryYoutube query
        | Some result => promisify result
    };
};

let getUserPlaylists () => {
    Playlists.list @@ Playlists.listOptions part::"id,snippet" mine::Js.true_ ();
};

let createPlaylist name => {
    Playlists.insert @@ Playlists.insertOptions part::"id,snippet" title::name ();
};

let insertPlaylistItem playlistId videoId => {
    PlaylistItems.insert @@ PlaylistItems.insertOptions playlistId::playlistId videoId::videoId ();
};