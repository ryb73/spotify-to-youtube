/* Deduping this later when not lazy */

type urlObject;

[@bs.obj]
external makeUrl : (~pathname: string, ~protocol: string, ~slashes: Js.boolean, unit) => urlObject =
  "";

[@bs.module "url"] external formatUrl : urlObject => string = "format";
