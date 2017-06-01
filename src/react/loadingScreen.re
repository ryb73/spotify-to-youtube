module LoadingScreen = {
    include ReactRe.Component;
    let name = "LoadingScreen";
    type props = { message: string };

    let render { props } =>
        <div>
            <i className="fa fa-refresh fa-spin" />
            (ReactRe.stringToElement props.message)
        </div>;
};

include ReactRe.CreateComponent LoadingScreen;

let createElement ::message => wrapProps { message: message };