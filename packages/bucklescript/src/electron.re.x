module WebContents = {

};

module BrowserWindow = {
  type t;

  external _create : int => int => Js.t t = "BrowserWindow" [@@bs.module "electron"];
  external _loadUrl : Js.t t => string => unit = "loadURL" [@@bs.send];

  class c bw => {
    pub loadUrl url => {
      _loadUrl bw url;
    };

    pub getWebContents => bw##webContents;
  };

  let create width height => {
    (new c) (_create width height);
  };
};