open Electron;
open Node;
open MyNode;

external newBrowserWindow : int => int => Js.t BrowserWindow.t = "BrowserWindow" [@@bs.module "electron"];

let createWindow () => {
    let win = newBrowserWindow 800 600;

    let url = Url.format [%bs.obj
        {
        }
    ]
    win##loadURL (Path.join [| "/var/", "index.html" |]);
};
