open Electron;

[@bs.val] external dirname : string = "__dirname";

[%bs.raw "require('electron-debug')({showDevTools: true})"];

let winRef: ref(option(Js.t(BrowserWindow.t))) = ref(None);

let createWindow = () => {
  ElectronUpdater.checkForUpdatesAndNotify ();

  let win = Electron.BrowserWindow.mk();
  winRef := Some(win);
  win##loadURL("http://localhost:54380/");
  win##on(
    "closed",
    () => {
      winRef := None;
      Electron.app##quit()
    }
  );
  ()
};

Electron.app##on("ready", createWindow);

Electron.app##on(
  "activate",
  () =>
    switch winRef^ {
    | None => createWindow()
    | Some(_) => ()
    }
);

let server =
  HttpServer.createServer({
    "root": Node.Path.join([|dirname, "/../../../html/"|]),
    "cache": Js.Undefined.return(- 1) /* No caching */
  });

server##listen(54380);
