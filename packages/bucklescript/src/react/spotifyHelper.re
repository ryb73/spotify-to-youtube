let create () => Spotify.create [%bs.obj {
    clientId: "35988437b3404ab08d67eaced43c4997",
    clientSecret: Js.Undefined.return "secret",
    redirectUri: "http://localhost:54380/"
}];