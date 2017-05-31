open Js.Promise;

external encodeURIComponent : string => string = "" [@@bs.val];

type match 'a = {
    video: 'a,
    matchType: [
        | `OfficialVideo
        | `OfficialLyricVideo
        | `LyricVideo
        | `AudioOnly
        | `Acoustic
        | `Live
        | `LiveInStudio
        | `Unknown
    ]
};

type csvRow = {.
    artist: string,
    song: string,
    matchType: string,
    videoTitle: string,
    videoUrl: string,
    searchUrl: string
};

let getTrackArtist track => (track##track##artists).(0)##name;

let getTrackSearchUrl track => "https://www.youtube.com/results?search_query=" ^
    (encodeURIComponent @@ getTrackArtist track ^ " " ^ track##track##name);

let emptyResult track => resolve [%bs.obj {
    artist: getTrackArtist track,
    song: track##track##name,
    matchType: "Not Found",
    videoTitle: "",
    videoUrl: "",
    searchUrl: getTrackSearchUrl track
}];

let idToLink = (^) "https://youtu.be/";

let matchTypeString matchType => switch matchType {
    | `OfficialVideo => "Official Video"
    | `OfficialLyricVideo => "Official Lyric Video"
    | `LyricVideo => "Lyric Video"
    | `Acoustic => "Acoustic"
    | `Live => "Live"
    | `LiveInStudio => "Live in Studio"
    | `AudioOnly => "Audio Only"
    | `Unknown => "Unknown"
};

let videoResult track video matchType => resolve [%bs.obj {
    artist: getTrackArtist track,
    song: track##track##name,
    matchType: matchTypeString matchType,
    videoTitle: video##snippet##title,
    videoUrl: idToLink video##id##videoId,
    searchUrl: getTrackSearchUrl track
}];

let sanitize str => {
    let result = str
        |> Js.String.toLowerCase
        |> Js.String.replaceByRe [%bs.re "/[^a-z0-9\\s]/g"] "";

    switch (Js.String.length result) {
        | 0 => str
        | _ => result
    };
};

let filterBadResults track searchResults => {
    let sanitizedArtist = sanitize @@ getTrackArtist track;
    let sanitizedTrack = sanitize track##track##name;

    searchResults
        |> Js.Array.filter (fun video => {
            let sanitizedTitle = sanitize video##snippet##title;
            (Js.String.includes sanitizedArtist sanitizedTitle) &&
                (Js.String.includes sanitizedTrack sanitizedTitle);
        });
};

let (<||>) a b => switch (a,b) {
    | (Some _, _) => a
    | (_, Some _) => b
    | (None, None) => None
};

let searchForKeyword track video keyword => {
    let searchString = (sanitize @@ getTrackArtist track) ^
        (sanitize track##track##name);

    let sanitizedTitle = sanitize video##snippet##title;

    /* If the song title contains the keyword, then consider it a non match to avoid
        false positives */
    if (Js.String.includes keyword searchString) {
        false;
    } else {
        let regexStr = "(^|[^a-z])" ^ keyword ^ "($|[^a-z])";
        let regex = Js.Re.fromStringWithFlags regexStr flags::"i";

        switch (Js.String.match_ regex sanitizedTitle) {
            | Some _ => true
            | _ => false
        };
    }
};

let searchForAnyKeywords track video keywords => {
    Js.Array.some (searchForKeyword track video) keywords;
};

let searchForAllKeywords track video keywords => {
    Js.Array.every (searchForKeyword track video) keywords;
};

let classifyVideo track video : match 'a => {
    let matchType =
        searchForKeyword track video "acoustic" ? `Acoustic
        : searchForAnyKeywords track video [| "studio", "sessions?" |] ? `LiveInStudio
        : searchForKeyword track video "live" ? `Live
        : searchForKeyword track video "audio" ? `AudioOnly
        : searchForAllKeywords track video [| "lyrics?", "official" |] ? `OfficialLyricVideo
        : searchForKeyword track video "lyrics?" ? `LyricVideo
        : searchForKeyword track video "official" ? `OfficialVideo
        : video##snippet##channelTitle
                |> Js.String.toLowerCase
                |> Js.String.includes "vevo" ? `OfficialVideo
        : `Unknown;

    { video, matchType };
};

let findVideoOfType matchType matches => {
    matches |>
        Js.Array.find (fun match => {
            match.matchType === matchType;
        });
};

let resolveBestMatch track videoSearchResults => {
    let filteredResults = filterBadResults track videoSearchResults;
    if(Js.Array.length filteredResults === 0) {
        emptyResult track;
    } else {
        let classifications = Js.Array.map (classifyVideo track) filteredResults;

        let match =
            findVideoOfType `OfficialVideo classifications <||>
            findVideoOfType `OfficialLyricVideo classifications <||>
            findVideoOfType `Unknown classifications <||>
            findVideoOfType `LyricVideo classifications <||>
            findVideoOfType `Acoustic classifications <||>
            findVideoOfType `Live classifications;

        switch match {
            | Some { video, matchType } => videoResult track video matchType
            | None => emptyResult track
        };
    }
};

let matchTrack ytHelper track => {
    let artistName = getTrackArtist track;
    let trackName = track##track##name;
    YouTubeHelper.doSearch ytHelper (artistName ^ " " ^ trackName)
        |> then_ (fun data => {
            resolveBestMatch track data##result##items;
        });
};
