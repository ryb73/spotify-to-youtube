open Js.Promise;

module PromptConnectYouTube = {
    include ReactRe.Component.Stateful;
    let name = "PromptConnectYouTube";
    type props = { onSignedIn : YouTubeHelper.t => unit };
    type state = { initialized: bool };

    let getInitialState _ => { initialized: false };

    let signInStatusChanged { props } ytHelper signedIn => {
        if(signedIn) {
            props.onSignedIn ytHelper;
        } else {
            ();
        }
    };

    let componentDidMount bag => {
        let { setState, props } = bag;

        YouTubeHelper.init ()
            |> then_ (fun ytHelper => {
                setState (fun { state } => { ...state, initialized: true });

                if(YouTubeHelper.isSignedIn ()) {
                    props.onSignedIn ytHelper;
                } else {
                    YouTubeHelper.listenSignInChange @@ signInStatusChanged bag ytHelper;
                };

                resolve ();
            });

        None;
    };

    let loginClicked _ => {
        YouTubeHelper.signIn ();
    };

    let renderBody bag => {
        let { state } = bag;

        if(state.initialized) {
            <div>
                <a href="#" onClick=loginClicked>(ReactRe.stringToElement "Log In")</a>
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
