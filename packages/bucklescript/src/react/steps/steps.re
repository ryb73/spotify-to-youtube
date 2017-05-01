module Steps = {
    include ReactRe.Component;
    let name = "Steps";
    type props = {
        whichStep: [ `ConnectSpotify | `ConnectYoutube ]
    };

    type step = {
        message: string,
        body: ReactRe.reactElement
    };

    let steps = [{
        message: "Step 1: Log into Spotify",
        body: <PromptConnectSpotify />
    }, {
        message: "Step 2: Log into Youtube",
        body: <PromptConnectYoutube />
    }];

    let render { props } => {
        <div>

        </div>;
    };
};

include ReactRe.CreateComponent Steps;

let createElement ::whichStep => wrapProps { whichStep: whichStep };