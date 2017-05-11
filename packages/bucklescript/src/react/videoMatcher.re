open Js.Promise;

type match 'a = VidMatch 'a string
                | NoMatch;

external encodeURIComponent : string => string = "" [@@bs.val];

let headers = [%bs.obj {
    artist: "Artist",
    song: "Song",
    matchCategory: "Match Category",
    videoTitle: "Video Title",
    videoUrl: "Video URL",
    searchUrl: "Search URL"
}];

let getTrackArtist track => (track##track##artists).(0)##name;

let getTrackSearchUrl track => "https://www.youtube.com/results?search_query=" ^
    (encodeURIComponent @@ getTrackArtist track ^ " " ^ track##track##name);

let emptyResult track => resolve [%bs.obj {
    artist: getTrackArtist track,
    song: track##track##name,
    matchCategory: "Not Found",
    videoTitle: "",
    videoUrl: "",
    searchUrl: getTrackSearchUrl track
}];

let idToLink = (^) "https://youtu.be/";

let videoResult track video matchCategory => resolve [%bs.obj {
    artist: getTrackArtist track,
    song: track##track##name,
    matchCategory,
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
            [%bs.debugger];
            let sanitizedTitle = sanitize video##snippet##title;
            (Js.String.includes sanitizedArtist sanitizedTitle) &&
                (Js.String.includes sanitizedTrack sanitizedTitle) &&
                (not @@ Js.String.includes "audio" sanitizedTitle);
        });
};

let toMatch matchType video => switch video {
    | None => NoMatch
    | Some v => VidMatch v matchType
};

let (<||>) a b => switch (a,b) {
    | (VidMatch _ _, _) => a
    | (_, VidMatch _ _) => b
    | (NoMatch, NoMatch) => NoMatch
};

/* Includes lyric videos */
let findAnyOfficialVideo matchType videos => {
    /* Look for video with "Offical" in the name */
    let attempt1 = videos
        |> Js.Array.find (fun video => {
            let title = video##snippet##title;
            let match = Js.String.match_ [%bs.re "/(^|[^a-z])official/i"] title;
            switch match {
                | None => false
                | Some _ => true
            };
        })
        |> toMatch matchType;

    /* Look for a Vevo video */
    let attempt2 = videos
        |> Js.Array.find (fun video => {
            video##snippet##channelTitle
                |> Js.String.toLowerCase
                |> Js.String.includes "vevo";
        })
        |> toMatch matchType;

    attempt1 <||> attempt2;
};

let getTitlesIncluding str =>
    Js.Array.filter (fun video => {
        Js.String.includes str (sanitize video##snippet##title);
    });

let getTitlesNotIncluding str =>
    Js.Array.filter (fun video => {
        not @@ Js.String.includes str (sanitize video##snippet##title);
    });

let findOfficialVideo videos => {
    getTitlesNotIncluding "lyric" videos
        |> findAnyOfficialVideo "Official Video";
};

let findOfficialLyricVideo videos => {
    getTitlesIncluding "lyric" videos
        |> findAnyOfficialVideo "Official Lyric Video";
};

let getFirstMatch matchType videos => {
    switch (Js.Array.length videos) {
        | 0 => NoMatch
        | _ => VidMatch videos.(0) matchType;
    };
};

let findLyricVideo videos => {
    getTitlesIncluding "lyric" videos
        |> getFirstMatch "Lyric Video";
};

let findPossibleOfficialVideo track videos => {
    /* Try to filter out live videos unless the track title/artist actually contains
        the word "live"  */
    let searchString = (sanitize @@ getTrackArtist track) ^
        (sanitize track##track##name);
    let filteredResults = switch (Js.String.includes "live" searchString) {
        | false => getTitlesNotIncluding "live" videos
        | true => videos
    };

    getTitlesNotIncluding "lyric" filteredResults
        |> getFirstMatch "Possible Official Video";
};

let resolveBestMatch track videoSearchResults => {
    let filteredResults = filterBadResults track videoSearchResults;
    if(Js.Array.length filteredResults === 0) {
        emptyResult track;
    } else {
        let match =
            findOfficialVideo filteredResults <||>
            findOfficialLyricVideo filteredResults <||>
            findPossibleOfficialVideo track filteredResults <||>
            findLyricVideo filteredResults;

        switch match {
            | VidMatch video matchType => videoResult track video matchType
            | NoMatch => emptyResult track
        };
    }
};

let matchTrack track => {
    let artistName = getTrackArtist track;
    let trackName = track##track##name;
    YouTubeHelper.doSearch (artistName ^ " " ^ trackName)
        |> then_ (fun (data:Js.t Google.YouTube.Search.result) => {
            let items = data##result##items;

            if(Js.Array.length items === 0) {
                emptyResult track;
            } else {
                resolveBestMatch track items;
            }
        });
};
