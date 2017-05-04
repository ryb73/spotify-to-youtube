open Js.Promise;

module PlaylistList = {
    include ReactRe.Component.Stateful;
    let name = "PlaylistList";
    type props = { spotify: Js.t Spotify.t };

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

    let renderPlaylists playlists => {
        let listItems = playlists
            |> Js.Array.map (fun playlist => {
                <li key=playlist##id>(ReactRe.stringToElement playlist##name)</li>
            });

        <div>
            <p>(ReactRe.stringToElement "Select a playlist below:")</p>
            <ul>
                (ReactRe.arrayToElement listItems)
            </ul>
        </div>
    };

    let renderBody { state } => {
        switch state.errorMessage {
            | Some msg => <span>(ReactRe.stringToElement ("Error: " ^ msg))</span>;
            | None =>
                switch state.playlists {
                    | None => <LoadingScreen message="Connecting to Spotify" />;
                    | Some playlists => renderPlaylists playlists;
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

let createElement ::spotify => wrapProps { spotify: spotify };