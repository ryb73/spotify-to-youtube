/* Deduping this later when not lazy */

type urlObject = [%bs.obj:
  {
    .
    pathname: string,
    protocol: string,
    slashes: Js.boolean
  }
] [@bs];

external formatUrl : urlObject => string = "format" [@@bs.module "url"];