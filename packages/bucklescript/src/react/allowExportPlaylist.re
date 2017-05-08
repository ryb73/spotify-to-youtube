open Js.Promise;

module AllowExportPlaylist = {
    include ReactRe.Component.Stateful;
    let name = "AllowExportPlaylist";
    type props = {
        spotify: Js.t Spotify.t,
        playlistId: string
    };

    type state = {
        statusMessage: option string,
        errorMessage: option string
    };

    let getInitialState _ => { statusMessage: None, errorMessage: None };

    let getTracks { props } => SpotifyHelper.getPlaylistTracks props.spotify props.playlistId;

    let matchTrack track => resolve track;

    let formatMatchedTrack track => {
        resolve [%bs.obj {
            artist: (track##track##artists).(0)##name,
            song: track##track##name
        }];
    };

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

                matchTrack track
                    |> then_ formatMatchedTrack
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
                    ...state, statusMessage: Some "Complete!"
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

    let skipStep _ _ => None;

    let renderOptions { updater } =>
        <p>
            <a href="#" onClick=(updater beginMatching)>
                (ReactRe.stringToElement "Begin matching songs to videos")
            </a>
            (ReactRe.stringToElement " | ")
            <a href="#" onClick=(updater skipStep)>
                (ReactRe.stringToElement {js|I already have a CSV – Skip this step|js})
            </a>
        </p>;

    let renderErrorMessage { state } => {
        switch state.errorMessage {
            | None => ReactRe.stringToElement ""
            | Some msg => ReactRe.stringToElement @@ msg
        };
    };

    let renderStatusSection bag => {
        let { state } = bag;

        switch state.statusMessage {
            | Some msg => (ReactRe.stringToElement @@ "!" ^ msg)
            | None =>
                <div>
                    (renderOptions bag)
                    (renderErrorMessage bag)
                </div>
        };
    };

    let render bag => {
        <div>
            <h1>(ReactRe.stringToElement "Step 4: Match Songs to YouTube Videos")</h1>
            <p>
                (ReactRe.stringToElement @@ "A CSV file, which can be opened using Microsoft Excel or other similar " ^
                    "applications, will now be created. The file will include each song from your Spotify playlist " ^
                    "along with the best match on YouTube. You can modify the CSV to add/remove/modify any songs " ^
                    "you wish. The CSV will then be used in the next step to create the YouTube playlist.")
            </p>
            <p>
                (ReactRe.stringToElement @@ "If you've already completed this step and have a CSV ready, you can " ^
                    "skip this and move on to the next step.")
            </p>
            (renderStatusSection bag)
        </div>
    };
};

include ReactRe.CreateComponent AllowExportPlaylist;

let createElement ::spotify ::playlistId => wrapProps { spotify, playlistId };