open Js.Promise;

type cache;
external _initialize : unit => Js.Promise.t cache = "initialize" [@@bs.module "./youtube-cache"];
external _get : cache => string => Js.Promise.t (Js.null (Js.t Google.YouTube.Search.result)) = "get" [@@bs.send];
external _set : cache => string => Js.t Google.YouTube.Search.result => unit = "set" [@@bs.send];

let initialize = _initialize;

let get cache query => _get cache query
    |> then_  (fun result => {
        resolve @@ Js.Null.to_opt result;
    });

let set = _set;