// Generated by BUCKLESCRIPT VERSION 1.7.1, PLEASE EDIT WITH CARE
'use strict';

var Electron   = require("electron");
var HttpServer = require("http-server");

var winRef = [/* None */0];

function createWindow() {
  var win = new Electron.BrowserWindow();
  winRef[0] = /* Some */[win];
  win.loadURL("http://localhost:54380/");
  win.on("closed", function () {
        winRef[0] = /* None */0;
        return /* () */0;
      });
  return /* () */0;
}

Electron.app.on("ready", createWindow);

Electron.app.on("activate", function () {
      var match = winRef[0];
      if (match) {
        return /* () */0;
      } else {
        return createWindow(/* () */0);
      }
    });

var server = HttpServer.createServer({
      root: "html/"
    });

server.listen(54380);

exports.winRef       = winRef;
exports.createWindow = createWindow;
exports.server       = server;
/*  Not a pure module */
