type timeout;
external setTimeout : (unit => unit) => int => timeout = "" [@@bs.val];
external clearTimeout : timeout => unit = "" [@@bs.val];

let nextSave = ref None;

module CacheItem = FlatCache.Make({
    type t = Js.undefined (Js.t Google.YouTube.Search.result);
});

let cache = FlatCache.load "spotifyToYoutube";

let get query => Js.Undefined.to_opt @@ CacheItem.getKey cache query;

let queueSave () => {
    nextSave := Some (setTimeout (fun _ => {
        Js.log "caching";
        FlatCache.save cache;
        nextSave := None
    /*}) 30000);*/
    }) 300000);
};

let set query data => {
    CacheItem.setKey cache query (Js.Undefined.return data);

    /* Only save if there's not already one queued */
    switch !nextSave {
        | Some _ => ()
        | None => queueSave ()
    };
};