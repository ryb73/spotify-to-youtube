open Js.Promise;

open Google.YouTube;

[@bs.val] external setTimeout : (unit => unit, int) => unit = "";

type t = {cache: YouTubeCache.cache};

let clientId = "655237426898-8ge9gblffi9184ng84lqhm1il5p5ifaq.apps.googleusercontent.com";

let init = () =>
  Google.Client.init(clientId, [|`YouTubeSSL|])
  |> then_((_) => YouTubeCache.initialize())
  |> then_((cache) => resolve({cache: cache}));

let listenSignInChange = (listener) => {
  let auth = Google.Auth2.getAuthInstance();
  auth##isSignedIn##listen(listener)
};

let isSignedIn = () => {
  let auth = Google.Auth2.getAuthInstance();
  auth##isSignedIn##get()
};

let signIn = () => {
  let auth = Google.Auth2.getAuthInstance();
  auth##signIn()
};

let queryYoutube = ({cache}, query) => {
  let opts = {"part": "snippet", "q": query, "maxResults": Js.Undefined.return(20)};
  Search.list(opts)
  |> then_(
       (data) => {
         YouTubeCache.set(cache, query, data);
         resolve(data)
       }
     )
};

/* The UI thread won't get a chance to update without this */
let promisify = (v) =>
  Js.Promise.make(
    (~resolve, ~reject as _) => {
      let timeoutFun = (_) => [@bs] resolve(v);
      setTimeout(timeoutFun, 1)
    }
  );

let doSearch = (ytHelper, query) =>
  YouTubeCache.get(ytHelper.cache, query)
  |> then_(
       (cachedValue) =>
         switch cachedValue {
         | None => queryYoutube(ytHelper, query)
         | Some(result) => promisify(result)
         }
     );

let getUserPlaylists = () =>
  Playlists.list @@ Playlists.listOptions(~part="id,snippet", ~mine=Js.true_, ());

let createPlaylist = (name) => {
  Js.log("createPlaylist");
  Playlists.insert @@ Playlists.insertOptions(~part="id,snippet", ~title=name, ());
};

let insertPlaylistItem = (playlistId, videoId) =>
  PlaylistItems.insert @@ PlaylistItems.insertOptions(~playlistId, ~videoId, ());
