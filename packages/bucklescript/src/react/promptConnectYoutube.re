module PromptConnectYoutube = {
    include ReactRe.Component;
    let name = "PromptConnectYoutube";
    type props = unit;

    let render _ => {
        <div>
            <h1>(ReactRe.stringToElement "Step 2: Log into YouTube")</h1>
            <p>
                <a href="#">(ReactRe.stringToElement "Log In")</a>
            </p>
        </div>;
    };
};

include ReactRe.CreateComponent PromptConnectYoutube;

let createElement = wrapProps ();