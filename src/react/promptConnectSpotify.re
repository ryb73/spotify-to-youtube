let s2e = ReasonReact.stringToElement;

let component = ReasonReact.statelessComponent("PromptConnectSpotify");

let make = (_) => {
  ...component,
  render: (_) => {
    let spotifyUrl = {
      let requiredPermissions = ["playlist-read-private", "playlist-read-collaborative"];
      SpotifyHelper.authUrl(requiredPermissions, "state")
    };
    <div>
      <h1> (s2e("Step 1: Log into Spotify")) </h1>
      <p> <a href=spotifyUrl> (s2e("Log In")) </a> </p>
    </div>
  }
};
