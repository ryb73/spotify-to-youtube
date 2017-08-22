open Electron;

external dirname : string = "__dirname" [@@bs.val];

[%bs.raw "require('electron-debug')({showDevTools: true})"];

let winRef : ref (option (Js.t BrowserWindow.t)) = ref None;

let createWindow () => {
    let win = Electron.BrowserWindow.mk ();
    winRef := Some win;

    win##loadURL "http://localhost:54380/";

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

let server = HttpServer.createServer [%bs.obj {
    root: Node.Path.join [| dirname, "/../../../html/" |],
    cache: Js.Undefined.return ~-1 /* No caching */
}];
server##listen 54380;