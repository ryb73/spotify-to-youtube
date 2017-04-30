open Js.Promise;

module PlaylistList = {
    include ReactRe.Component.Stateful;
    let name = "PlaylistList";
    type props = { spotify: Js.t Spotify.t };

    type state = {
        userId: option string,
        playlists: option (array (Js.t Spotify.playlist))
    };

    let getInitialState _ => {
        userId: None,
        playlists: None
    };

    let setPlaylists { setState } data => {
        setState (fun { state } =>
            { ...state, playlists: Some data##body##items }
        );

        resolve ();
    };

    let setUserId bag data => {
        let { setState, props } = bag;

        setState (fun { state } =>
            { ...state, userId: Some data##body##id }
        );

        props.spotify##getUserPlaylists data##body##id
            |> then_ (setPlaylists bag);

        resolve ();
    };

    let componentDidMount bag => {
        let { props } = bag;

        props.spotify##getMe ()
            |> then_ (setUserId bag);

        None;
    };

    let renderPlaylists playlists => {
        let listItems = playlists
            |> Js.Array.map (fun playlist => {
                <li key=playlist##id>(ReactRe.stringToElement playlist##name)</li>
            });

        <ul>
            (ReactRe.arrayToElement listItems)
        </ul>
    };

    let render { state } => {
        switch (state.playlists) {
            | None => <LoadingScreen message="Connecting to Spotify" />;
            | Some playlists => renderPlaylists playlists;
        };
    };
};

include ReactRe.CreateComponent PlaylistList;

let createElement ::spotify => wrapProps { spotify: spotify };