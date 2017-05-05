open Js.Promise;

module PromptConnectYouTube = {
    include ReactRe.Component.Stateful;
    let name = "PromptConnectYouTube";
    type props = { onSignedIn : unit => unit };
    type state = { initialized: bool };

    let getInitialState _ => { initialized: false };

    let signInStatusChanged { props } signedIn => {
        if(signedIn) {
            props.onSignedIn ();
        } else {
            ();
        }
    };

    let componentDidMount bag => {
        let { setState, props } = bag;

        YoutubeHelper.init ()
            |> then_ (fun _ => {
                setState (fun { state } => { ...state, initialized: true });

                let _ = if(YoutubeHelper.isSignedIn ()) {
                    props.onSignedIn ();
                } else {
                    YoutubeHelper.listenSignInChange @@ signInStatusChanged bag;
                }

                resolve ();
            });

        None;
    };

    let loginClicked bag _ => {
        None;
    };

    let renderBody bag => {
        let { state, updater } = bag;

        if(state.initialized) {
            <div>
                <a href="#" onClick=(updater loginClicked)>(ReactRe.stringToElement "Log In")</a>
            </div>;
        } else {
            <div>
                (ReactRe.stringToElement "Loading...")
            </div>;
        }
    };

    let render bag => {
        <div>
            <h1>(ReactRe.stringToElement "Step 3: Log into YouTube")</h1>
            (renderBody bag)
        </div>;
    };
};

include ReactRe.CreateComponent PromptConnectYouTube;

let createElement ::onSignedIn => wrapProps { onSignedIn: onSignedIn };