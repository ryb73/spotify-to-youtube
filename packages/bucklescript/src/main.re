open Electron;
open Node;
open MyNode;

let createWindow () => {
    let win = BrowserWindow.create 800 600;

    let url = Url.formatUrl [%bs.obj
        {
            pathname: Path.join [| "/var/", "index.html" |],
            protocol: "file:",
            slashes: Js.true_
        }
    ];

    win##loadURL url;

    (win##getWebContents ())
        ## openDevTools ();

    win##on BrowserWindow.Closed (fun () => ());

    ();
};

Electron.app##on Electron.App.Ready createWindow;