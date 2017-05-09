open Js.Promise;

let s2e = ReactRe.stringToElement;

module AllowExportPlaylist = {
    include ReactRe.Component.Stateful;
    let name = "AllowExportPlaylist";
    type props = {
        spotify: Js.t Spotify.t,
        playlistId: string
    };

    type state = {
        statusMessage: option string,
        errorMessage: option string,
        matchingFinished: bool
    };

    let getInitialState _ => { statusMessage: None, errorMessage: None, matchingFinished: false };

    let getTracks { props } => SpotifyHelper.getPlaylistTracks props.spotify props.playlistId;

    let writeMatch csvStream formattedMatch => {
        FastCsv.write csvStream formattedMatch;
        resolve ();
    };

    let updateStatusForTrack { setState } track => {
        let message = (track##track##artists).(0)##name ^ {js| – |js} ^
            track##track##name;

        setState (fun { state } => {
            { ...state, statusMessage: Some message }
        });
    };

    /* Search for (and write) a match for each track in series */
    let rec matchEachTrack bag csvStream tracks => {
        switch tracks {
            | [] => resolve ()
            | [ track, ...remaining ] => {
                updateStatusForTrack bag track;

                VideoMatcher.matchTrack track
                    |> then_ @@ writeMatch csvStream
                    |> then_ (fun _ => matchEachTrack bag csvStream remaining)
            }
        };
    };

    let openCsvFile bag filename => {
        Node.Fs.createWriteStream filename Js.undefined (fun fileStream => {
            FastCsv.createWriteStream Js.undefined (fun csvStream => {
                FastCsv.pipe csvStream fileStream;
                getTracks bag
                    |> then_ (fun arr => resolve @@ Array.to_list arr)
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

    let goToNextStep _ _ => None;

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

let createElement ::spotify ::playlistId => wrapProps { spotify, playlistId };