open Js.Promise;

module PlaylistList = {
    include ReactRe.Component.Stateful;
    let name = "PlaylistList";
    type props = {
        spotify: Js.t Spotify.t,
        playlistSelected: string => unit
    };

    type state = {
        playlists: option (array (Js.t Spotify.playlist)),
        errorMessage: option string
    };

    let getInitialState _ => {
        playlists: None,
        errorMessage: None
    };

    let setPlaylists { setState } items => {
        setState (fun { state } =>
            { ...state, playlists: Some items }
        );

        resolve ();
    };

    let setError { setState } errorMessage => {
        setState (fun { state } =>
            { ...state, errorMessage: Some errorMessage }
        );
    };

    let componentDidMount bag => {
        let { props } = bag;

        SpotifyHelper.getMyPlaylists props.spotify
            |> then_ (setPlaylists bag)
            |> catch (fun err => {
                resolve err; /* prevent from being cleaned up */
                setError bag [%bs.raw "err.message" ];
                resolve ();
            });

        None;
    };

    let playlistSelected playlistId { props } e => {
        ReactEventRe.Mouse.preventDefault e;

        props.playlistSelected playlistId;

        None;
    };

    let renderPlaylist { updater } playlist => {
        <li key=playlist##id>
            <a href="#" onClick=(updater (playlistSelected playlist##id))>(ReactRe.stringToElement playlist##name)</a>
        </li>
    };

    let renderPlaylists bag playlists => {
        let listItems = playlists
            |> Js.Array.map (renderPlaylist bag);

        <div>
            <p>(ReactRe.stringToElement "Select a playlist below:")</p>
            <ul>
                (ReactRe.arrayToElement listItems)
            </ul>
        </div>
    };

    let renderBody bag => {
        let { state } = bag;

        switch state.errorMessage {
            | Some msg => <span>(ReactRe.stringToElement ("Error: " ^ msg))</span>;
            | None =>
                switch state.playlists {
                    | None => <LoadingScreen message="Connecting to Spotify" />;
                    | Some playlists => renderPlaylists bag playlists;
                };
        };
    };

    let render bag => {
        <div>
            <h1>(ReactRe.stringToElement "Step 2: Select a playlist to convert")</h1>
            (renderBody bag)
        </div>
    };
};

include ReactRe.CreateComponent PlaylistList;

let createElement ::spotify ::playlistSelected => wrapProps { spotify, playlistSelected };