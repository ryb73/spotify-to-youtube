type urlObject = [%bs.obj:
  {
    .
    pathname: string,
    protocol: string,
    slashes: bool
  }
];

external formatUrl : urlObject => string = "format" [@@bs.module "url"];