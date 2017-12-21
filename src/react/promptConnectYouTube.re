open Js.Promise;

let s2e = ReasonReact.stringToElement;

type state = {initialized: bool};
type action = SetInitialized;

let signInStatusChanged = (onSignedIn, ytHelper, signedIn) =>
    if (signedIn) {
      onSignedIn(ytHelper)
    } else {
      ()
    };

let loginClicked = (_) => YouTubeHelper.signIn();

let renderBody = ({ ReasonReact.state }) => {
    if (state.initialized) {
    <div> <a href="#" onClick=loginClicked> (s2e("Log In")) </a> </div>
    } else {
    <div> (s2e("Loading...")) </div>
    }
};

let go = (reduce, action) => reduce((_) => action, ());

let component = ReasonReact.reducerComponent("PromptConnectYouTube");

let make = (~onSignedIn, _) => {
  ...component,
  initialState: () => {initialized: false},

  didMount: ({ reduce }) => {
    YouTubeHelper.init()
        |> then_((ytHelper) => {
           go(reduce, SetInitialized);
           if (YouTubeHelper.isSignedIn()) {
             onSignedIn(ytHelper)
           } else {
             YouTubeHelper.listenSignInChange @@ signInStatusChanged(onSignedIn, ytHelper)
           };
           resolve()
        });

    ReasonReact.NoUpdate;
  },

  render: (bag) =>
    <div>
      <h1> (s2e("Step 3: Log into YouTube")) </h1>
      (renderBody(bag))
    </div>,

  reducer: (_, _) => ReasonReact.Update { initialized: true }
};
