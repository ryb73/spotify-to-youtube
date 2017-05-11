module CacheItem = FlatCache.Make({
    type t = Js.undefined (Js.t Google.YouTube.Search.result);
});

let cache = FlatCache.load "spotifyToYoutube";

let get query => Js.Undefined.to_opt @@ CacheItem.getKey cache query;

let set query data => {
    CacheItem.setKey cache query (Js.Undefined.return data);
    FlatCache.save cache;
};