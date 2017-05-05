let clientId = "655237426898-8ge9gblffi9184ng84lqhm1il5p5ifaq.apps.googleusercontent.com";

let init () => Google.Client.init clientId [| `YouTubeSSL |];

let listenSignInChange listener => {
    let auth = Google.Auth2.getAuthInstance ();
    auth##isSignedIn##listen listener [@bs];
};

let isSignedIn () => {
    /*let auth = Google.Auth2.getAuthInstance ();
    auth##isSignedIn##get () [@bs];*/
    true;
};