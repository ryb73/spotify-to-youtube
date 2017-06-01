open Js.Promise;

module CsvFormatter = FastCsv.Format({
    type t = Js.t VideoMatcher.csvRow;
});

let s2e = ReactRe.stringToElement;

module AllowExportPlaylist = {
    include ReactRe.Component.Stateful;
    let name = "AllowExportPlaylist";
    type props = {
        spotify: Js.t Spotify.t,
        playlistId: string,
        onNextStep: unit => unit,
        ytHelper: YouTubeHelper.t
    };

    type state = {
        statusMessage: option string,
        errorMessage: option string,
        matchingFinished: bool
    };

    let getInitialState _ => { statusMessage: None, errorMessage: None, matchingFinished: false };

    let getTracks { props } => SpotifyHelper.getPlaylistTracks props.spotify props.playlistId;

    let writeMatch csvStream formattedMatch => {
        CsvFormatter.write csvStream formattedMatch;
        resolve ();
    };

    let updateStatusForTrack { setState } track currentIndex numRemaining => {
        let artist = (track##track##artists).(0)##name;
        let song = track##track##name;
        let currentTrack = currentIndex + 1;
        let totalTracks = currentTrack + numRemaining;
        let message = {j|[ $currentTrack / $totalTracks ] $artist – $song|j};

        setState (fun { state } => {
            { ...state, statusMessage: Some message }
        });
    };

    /* Search for (and write) a match for each track in series */
    let rec matchEachTrack bag csvStream tracks => {
        let { props } = bag;

        switch tracks {
            | [] => resolve ()
            | [ (track, index), ...remaining ] => {
                updateStatusForTrack bag track index (List.length tracks);

                VideoMatcher.matchTrack props.ytHelper track
                    |> then_ @@ writeMatch csvStream
                    |> then_ (fun _ => matchEachTrack bag csvStream remaining)
            }
        };
    };

    let openCsvFile bag filename => {
        let headers = {
            "artist": "Artist",
            "song": "Song",
            "matchType": "Match Category",
            "videoTitle": "Video Title",
            "videoUrl": "Video URL",
            "searchUrl": "Search URL"
        };

        Node.Fs.FileWriteStream.create filename Js.undefined (fun fileStream => {
            CsvFormatter.create Js.undefined (fun csvStream => {
                CsvFormatter.pipe csvStream (Node.Fs.FileWriteStream.writeable fileStream);

                CsvFormatter.write csvStream headers;

                getTracks bag
                    |> then_ (fun trackArr => {
                        trackArr
                            |> Js.Array.mapi (fun track i => (track, i))
                            |> Array.to_list
                            |> resolve;
                    })
                    |> then_ @@ matchEachTrack bag csvStream;
            });
        })
    };

    let beginMatching bag _ => {
        let { state, setState } = bag;

        let outputFilename = Electron.Electron.remote##dialog##showSaveDialog Js.undefined Js.undefined;

        openCsvFile bag outputFilename
            |> then_ (fun _ => {
                setState (fun { state } => {
                    ...state, statusMessage: None, matchingFinished: true
                });

                resolve ();
            })
            |> catch (fun err => {
                Js.log err;

                let message = "An error occurred. Please make sure you have the ability to save to the location you chose.";

                setState (fun { state } => {
                    ...state, errorMessage: Some message, statusMessage: None
                });

                resolve ();
            });

        Some { ...state, statusMessage: Some "Loading playlist tracks" };
    };

    let goToNextStep { props } _ => {
        props.onNextStep ();
        None;
    };

    let renderOptions { updater } =>
        <p>
            <a href="#" onClick=(updater beginMatching)>
                (s2e "Begin matching songs to videos")
            </a>
            (s2e " | ")
            <a href="#" onClick=(updater goToNextStep)>
                (s2e {js|I already have a CSV – Skip this step|js})
            </a>
        </p>;

    let renderErrorMessage { state } => {
        switch state.errorMessage {
            | None => s2e ""
            | Some msg => s2e msg
        };
    };

    let renderStatusSection bag => {
        let { state, updater } = bag;

        if(state.matchingFinished) {
            <span>
                (s2e "Complete! ")
                <a href="#" onClick=(updater goToNextStep)>(s2e "Click here to continue.")</a>
            </span>
        } else {
            switch state.statusMessage {
                | Some msg =>
                    <div>
                        <i className="fa fa-refresh fa-spin" />
                        (s2e @@ " " ^ msg)
                    </div>

                | None =>
                    <div>
                        (renderOptions bag)
                        (renderErrorMessage bag)
                    </div>
            };
        }
    };

    let render bag => {
        <div>
            <h1>(s2e "Step 4: Match Songs to YouTube Videos")</h1>
            <p>
                (s2e @@ "A CSV file, which can be opened using Microsoft Excel or other similar " ^
                    "applications, will now be created. The file will include each song from your Spotify playlist " ^
                    "along with the best match on YouTube. You can modify the CSV to add/remove/modify any songs " ^
                    "you wish. The CSV will then be used in the next step to create the YouTube playlist.")
            </p>
            <p>
                (s2e @@ "If you've already completed this step and have a CSV ready, you can " ^
                    "skip this and move on to the next step.")
            </p>
            (renderStatusSection bag)
        </div>
    };
};

include ReactRe.CreateComponent AllowExportPlaylist;

let createElement ::spotify ::playlistId ::onNextStep ::ytHelper =>
    wrapProps { spotify, playlistId, onNextStep, ytHelper };