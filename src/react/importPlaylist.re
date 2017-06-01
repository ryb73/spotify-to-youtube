open Js.Promise;

let s2e = ReactRe.stringToElement;

module ImportPlaylist = {
    include ReactRe.Component.Stateful;
    let name = "ImportPlaylist";
    type props = { playlistName: string };

    type status =
        | Ready
        | NamingConflict string (list (array string)) /* existingPlaylistId * csvData */
        | Processing string /* message */
        | Done;

    type state = {
        errors: array string,
        status: status
    };

    let getInitialState _ => { errors: [||], status: Ready };

    let artistColumn = 0;
    let songColumn = 1;
    let videoUrlColumn = 4;

    let validateCsv csv => {
        let csvList = Array.to_list csv;
        switch csvList {
            | [ headers, ...data ] => {
                (headers.(videoUrlColumn) === "Video URL") ?
                    Some data
                :
                    None;
            }

            | [] => None
        };
    };

    let setErrorMessage { setState } errorMessage => {
        setState (fun { state } => {
            state.errors |> Js.Array.push errorMessage;
            { ...state, errors: state.errors };
        });
    };

    let getVideoUrlFromRow row => row.(videoUrlColumn);

    let getVideoIdFromUrl url => {
        let pieces = Js.String.split "https://youtu.be/" url;

        /* Allow for blank or "not found" rows */
        if(Js.Array.length pieces < 2) {
            None;
        } else {
            Some pieces.(1);
        }
    };

    let getVideoNameFromRow row => {
        let artist = row.(artistColumn);
        let song = row.(songColumn);
        {j|$artist – $song|j};
    };

    let addVideoToPlaylist bag playlistId (videoId, videoName) => {
        let status = "Adding " ^ videoName;
        bag.setState (fun { state } => { ...state, status: Processing status });

        switch videoId {
            | None => resolve ()
            | Some id => {
                YouTubeHelper.insertPlaylistItem playlistId id
                    |> then_ (fun _ => resolve ())
                    |> catch (fun err => {
                        Js.log err;
                        setErrorMessage bag ("Error adding video " ^ videoName ^ " to playlist.");
                        resolve ();
                    });
            }
        };
    };

    let parseVideoRow row => {
        (
            row |> getVideoUrlFromRow |> getVideoIdFromUrl,
            row |> getVideoNameFromRow
        );
    };

    let addSongsToPlaylist bag playlistId data => {
        let { setState } = bag;

        data
            |> List.map parseVideoRow
            |> List.fold_left (fun promise videoInfo => {
                promise
                    |> then_ (fun _ => addVideoToPlaylist bag playlistId videoInfo);
            }) (resolve ())
            |> then_ (fun _ => {
                setState (fun { state } => { ...state, status: Done });
                resolve ();
            });
    };

    let findExistingPlaylistWithName name existingPlaylists => {
        let matchingPlaylist = existingPlaylists##result##items
            |> Js.Array.find (fun item => {
                item##snippet##title === name;
            });

        switch matchingPlaylist {
            | Some playlist => Some playlist##id
            | None => None
        };
    };

    let genericErrorHandler bag err => {
        Js.log err;
        setErrorMessage bag "There was an unknown error.";
        resolve ();
    };

    let beginPlaylistCreation bag (csvData, playlists) => {
        let { setState, props } = bag;

        switch (validateCsv csvData) {
            | Some validatedData => {
                switch (findExistingPlaylistWithName props.playlistName playlists) {
                    | Some playlistId => {
                        setState (fun { state } => {
                            ...state,
                            status: NamingConflict playlistId validatedData
                        });

                        resolve ();
                    }

                    | None => {
                        setState (fun { state } => {
                            ...state,
                            status: Processing "Creating playlist"
                        });

                        YouTubeHelper.createPlaylist props.playlistName
                            |> then_ @@ (fun data => addSongsToPlaylist bag data##result##id validatedData)
                            |> catch @@ genericErrorHandler bag;
                    }
                }
            }

            | None => {
                setErrorMessage bag "The file you've selected appears to have an invalid format.";
                resolve ();
            }
        };
    };

    let chooseCsv bag _ => {
        let inputFilename = Electron.Electron.remote##dialog##showOpenDialog Js.undefined Js.undefined;

        let readStream = Node.Fs.FileReadStream.create inputFilename.(0) Js.undefined;
        let csvStream = FastCsv.parse ();
        Node.Fs.FileReadStream.pipe readStream (FastCsv.Parse.writeable csvStream);

        let pCsv = StreamToArray.toArray @@ FastCsv.Parse.readable csvStream;
        let pPlaylists = YouTubeHelper.getUserPlaylists ();

        Js.Promise.all2 pCsv pPlaylists
            |> then_ @@ beginPlaylistCreation bag
            |> catch @@ genericErrorHandler bag;

        None;
    };

    let generatePlaylistName baseName => {
        let date = Js.Date.make ();
        let year = Js.Date.getFullYear date;
        let month = (Js.Date.getMonth date) +. 1.0;
        let day = Js.Date.getDate date;
        let hour = Js.Date.getHours date;
        let minute = Js.Date.getMinutes date;
        let second = Js.Date.getSeconds date;

        {j|$baseName $year-$month-$day $hour:$minute:$second|j};
    };

    let useDifferentPlaylistName csvData bag _ => {
        let { props, state } = bag;

        let newName = generatePlaylistName props.playlistName;
        YouTubeHelper.createPlaylist newName
            |> then_ @@ (fun data => addSongsToPlaylist bag data##result##id csvData)
            |> catch @@ genericErrorHandler bag;

        Some { ...state, status: Processing "Creating playlist" };
    };

    let useExistingPlaylist playlistId csvData bag _ => {
        addSongsToPlaylist bag playlistId csvData
            |> catch @@ genericErrorHandler bag;

        None;
    };

    let renderNameConflictPrompt { props, updater } playlistId csvData => {
        <p>
            (s2e @@ "A YouTube playlist with the name \"" ^ props.playlistName ^
                "\" already exists. What would you like to do? ")

            <a href="#" onClick=(updater @@ useDifferentPlaylistName csvData)>
                (s2e "Use a different name for the playlist")
            </a>
            (s2e " ")
            <a href="#" onClick=(updater @@ useExistingPlaylist playlistId csvData)>
                (s2e "Add to the existing playlist")
            </a>
        </p>;
    };

    let renderErrors errors => {
        let renderedErrors = errors
            |> Js.Array.mapi (fun message i => {
                <div key=(Js.String.make i)>(s2e message)</div>
            });

        <p>(ReactRe.arrayToElement renderedErrors)</p>;
    };

    let renderChooseLink { updater } => {
        <a href="#" onClick=(updater chooseCsv)>
            (s2e "Choose CSV file")
        </a>
    };

    let renderProcessingMessage message => {
        <p>
            <i className="fa fa-refresh fa-spin" />
            (s2e @@ " " ^ message)
        </p>
    };

    let renderFinishedMessage { state } => {
        let message = (Js.Array.length state.errors > 0) ?
            " Done, but there were some errors. Some songs might be missing from your playlist."
            : " Done";

        <p>
            <i className="fa fa-check" />
            (s2e message)
        </p>
    };

    let renderStatusSection bag => {
        let { state } = bag;

        let body =
            switch state.status {
                | Ready => renderChooseLink bag
                | NamingConflict playlistId csvData =>
                    renderNameConflictPrompt bag playlistId csvData
                | Processing message => renderProcessingMessage message
                | Done => renderFinishedMessage bag
            };

        <div>
            body
            (renderErrors state.errors)
        </div>;
    };

    let render bag =>
        <div>
            <h1>(s2e "Step 5: Import Playlist CSV")</h1>
            <p>
                (s2e @@ "The CSV you exported in the previous step will now be imported and the videos specified " ^
                    "in the file will be added to a new YouTube playlist.")
            </p>
            (renderStatusSection bag)
        </div>;
};

include ReactRe.CreateComponent ImportPlaylist;

let createElement ::playlistName => wrapProps { playlistName: playlistName };