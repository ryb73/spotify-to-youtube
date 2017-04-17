open Electron;
open Node;

type app;
type url;
/*type url = {
    .
    format [@bs.meth] : 
};*/

external app  : app  = "" [@@bs.module "electron"];
external url  : url  = "" [@@bs.module "url"];

external newBrowserWindow : int => int => Js.t BrowserWindow.t = "BrowserWindow" [@@bs.module "electron"];

let createWindow () => {
    let win = newBrowserWindow 800 600;
    
    win##loadURL (Path.join [| "/var/", "index.html" |]);
};
