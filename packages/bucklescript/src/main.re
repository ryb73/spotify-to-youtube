open Electron;

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

let server = HttpServer.createServer [%bs.obj { root: "html/" }];
server##listen 54380;