type urlObject = [%bs.obj:
  {
    .
    pathname: string,
    protocol: string,
    slashes: Js.boolean
  }
];

external formatUrl : urlObject => string = "format" [@@bs.module "url"];