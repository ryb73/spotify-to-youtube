open Electron;
open Node;
open MyNode;

/*type r = { contents: Js.null (Js.t BrowserWindow.t) };*/
external dirname : string = "__dirname" [@@bs.val];

let winRef : ref (option (Js.t BrowserWindow.t)) = ref None;

let createWindow () => {
    let win = Electron.BrowserWindow.mk ();
    winRef := Some win;

    let url = Url.formatUrl [%bs.obj
        {
            pathname: Path.join [| dirname, "../../../html/index.html" |],
            protocol: "file:",
            slashes: true
        }
    ];

    win##loadURL url;

    win##webContents##openDevTools ();

    win##on "closed" (fun () => {
        winRef := None
    });

    ();
};

Electron.app##on "ready" createWindow;

Electron.app##on "activate" (fun () => {
    switch !winRef {
        | None => createWindow ()
        | Some _ => ()
    };
});