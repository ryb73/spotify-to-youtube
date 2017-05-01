module Step = {
    include ReactRe.Component;
    let name = "Step";
    type props = {
        label: string,
        state: [ `Incomplete | `Complete | `InProgress ],
        children: Js.null_undefined ReactRe.reactJsChildren
    };

    type renderData = { headerStyle: ReactDOMRe.style, icon: ReactRe.reactElement };
    let getRenderData { props } => switch (props.state) {
        | `InProgress => {
            headerStyle: ReactDOMRe.Style.make color::"#000" (),
            icon: <i className="fa fa-square-o fa-3x fa-fw" />
        }

        | `Complete => {
            headerStyle: ReactDOMRe.Style.make color::"#777" (),
            icon:
                <span className="fa-stack fa-3x fa-fw">
                    <i className="fa fa-square-o fa-stack-2x" />
                    <i className="fa fa-check fa-stack-1x"
                        style=(ReactDOMRe.Style.make color::"green" ())
                    />
                </span>
        }

        | `Incomplete => {
            headerStyle: ReactDOMRe.Style.make color::"#777" (),
            icon: <i className="fa fa-square-o fa-3x fa-fw" />
        }
    };

    let render bag => {
        let { props } = bag;
        let { headerStyle, icon } = getRenderData bag;

        <div>
            <h1 style=headerStyle>(ReactRe.stringToElement props.label)</h1>
            (icon)
            <p>(ReactRe.listToElement @@ ReactRe.jsChildrenToReason props.children)</p>
        </div>
    };
};

include ReactRe.CreateComponent Step;

let createElement ::state ::label ::children  => wrapProps { state, label, children };