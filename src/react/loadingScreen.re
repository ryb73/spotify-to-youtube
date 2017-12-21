let component = ReasonReact.statelessComponent("LoadingScreen");

let make = (~message, _) => {
  ...component,
  render: (_) =>
    <div> <i className="fa fa-refresh fa-spin" /> (ReasonReact.stringToElement(message)) </div>
};
