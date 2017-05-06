open Js.Promise;

module AllowExportPlaylist = {
    include ReactRe.Component;
    let name = "AllowExportPlaylist";
    type props = { spotify: Js.t Spotify.t, playlistId: string };

    let beginMatching { props } _ => {
        SpotifyHelper.getPlaylistTracks props.spotify props.playlistId
            |> then_ (fun tracks => {
                tracks
                    |> Js.Array.forEach (fun track => {
                        Js.log @@ (track##track##artists).(0)##name ^ {js| – |js} ^
                            track##track##name;
                    });

                Electron.Electron.remote##dialog##showSaveDialog Js.undefined Js.undefined;

                resolve ();
            });

        None;
    };

    let skipStep _ _ => None;

    let render { updater } =>
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
            <p>
                <a href="#" onClick=(updater beginMatching)>
                    (ReactRe.stringToElement "Begin matching songs to videos")
                </a>
                (ReactRe.stringToElement " | ")
                <a href="#" onClick=(updater skipStep)>
                    (ReactRe.stringToElement {js|I already have a CSV – Skip this step|js})
                </a>
            </p>
        </div>;
};

include ReactRe.CreateComponent AllowExportPlaylist;

let createElement ::spotify ::playlistId => wrapProps { spotify, playlistId };