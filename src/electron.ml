module EventEmitter = struct
  module Event = struct
    type ('e, 'v) t = ..
    type (_,_) t +=
      | NewListener : ([> `EventEmitter], string) t
      | RemoveListener : ([> `EventEmitter], string) t
    type pkg = Pkg : ('e, 'v) t -> pkg
    external erase : ('e, 'v) t -> string = "%identity"
  end
  module Listener = struct
    type 'v t = 'v -> unit
    type pkg = Pkg : 'v t -> pkg
    module Elim = struct
      class type ['r] t = object
        method k : 'a. 'a -> 'r
      end [@bs]
      external mk : k:('a -> 'r) -> 'r t Js.t = "" [@@bs.obj]
    end
    external name : 'v t -> string = "name" [@@bs.get]
  end
  class type ['e] t = object
    method addListener : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method emit : ('e, 'v) Event.t -> 'v -> 'e t Js.t
    method eventNames : unit -> Event.pkg array
    method getMaxListeners : unit -> int
    method listenerCount : ('e, 'v) Event.t -> int
    method listeners : ('e, 'v) Event.t -> 'v Listener.t array
    method on : string -> 'v Listener.t -> 'e t Js.t
    method once : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method prependListener : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method prependOnceListener : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method removeAllListeners : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method removeListeners : ('e, 'v) Event.t -> 'v Listener.t -> 'e t Js.t
    method setMaxListeners : int -> 'e t Js.t
  end [@bs]
end

module WebContents = struct
  class type t = object
    method openDevTools : unit -> unit
  end [@bs]
end

module BrowserWindow = struct
  type ns = [`BrowserWindow | `EventEmitter]
  type (_,_) EventEmitter.Event.t +=
      | Closed : (ns, unit) EventEmitter.Event.t

  class type t = object
    inherit [ns] EventEmitter.t

    method loadURL : string -> unit
    method webContents : WebContents.t Js.t
  end [@bs]

  external create : int -> int -> t Js.t = "BrowserWindow" [@@bs.new] [@@bs.module "electron"]
end

module Certificate = struct type t end

module CommandLine = struct
  class type t = object
    method appendArgument : string -> unit
    method appendSwitch : string -> (* FIXME *) unit
  end [@bs]
end

module Event = struct type t end
module ImportCertificateOptions = struct type t end
module LoginAuthInfo = struct type t end
module LoginItemSettings = struct type t end
module LoginRequest = struct type t end
module Menu = struct type t end
module NativeImage = struct type t end
module Task = struct type t end

module Dock = struct
  module Bounce = struct
    type id = private int
    type kind = ..
    type kind +=
      | Critical
      | Informational
  end
  class type t = object
    method bounce : (* FIXME *) Bounce.kind -> Bounce.id
    method cancelBounce : Bounce.id -> unit
    method downloadFinished : string -> unit
    method getBadge : unit -> string
    (*method getBadgeCount : unit -> int*) (* NOTE: doesn't seem to work *)
    method hide : unit -> unit
    method setBadge : string -> unit
    (*method setBadgeCount : int -> bool*) (* NOTE: doesn't seem to work *)
    method setIcon : (* FIXME *) NativeImage.t Js.t -> unit
    method setMenu : Menu.t -> unit
    method show : unit -> unit
  end [@bs]
end

module Dialog = struct
  class type showSaveDialogOptions = object
    method title : string Js.undefined
    method defaultPath : string Js.undefined
    method buttonLabel : string Js.undefined
    (*method filters : FileFilter[] (optional)*)
    method message : string Js.undefined
    method nameFieldLabel : string Js.undefined
    method showsTagField : Js.boolean Js.undefined
  end [@bs]

  class type showOpenDialogOptions = object
  end [@bs]

  class type t = object
    method showSaveDialog : showSaveDialogOptions Js.undefined -> (string -> unit) Js.undefined -> string
    method showOpenDialog : showOpenDialogOptions Js.undefined -> (string array -> unit) Js.undefined -> string array
  end [@bs]
end

module Electron = struct
  module App = struct
    type ns = [`App | `EventEmitter]
    type (_,_) EventEmitter.Event.t +=
      | AccessibilitySupportChanged : (ns, Event.t * bool) EventEmitter.Event.t
      | Activate : (ns, unit) EventEmitter.Event.t
      | BeforeQuit : (ns, Event.t) EventEmitter.Event.t
      | BrowserWindowBlur : (ns, Event.t) EventEmitter.Event.t
      | BrowserWindowCreated : (ns, Event.t * BrowserWindow.t) EventEmitter.Event.t
      | BrowserWindowFocus : (ns, Event.t * BrowserWindow.t) EventEmitter.Event.t
      | ContinueActivity : (ns, Event.t * string * 'a Js.t) EventEmitter.Event.t
      | GpuProcessCrashed : (ns, unit) EventEmitter.Event.t
      | Login : (ns, Event.t * WebContents.t * LoginRequest.t * LoginAuthInfo.t * (string * string -> unit)) EventEmitter.Event.t
      | OpenFile : (ns, Event.t * string) EventEmitter.Event.t
      | OpenURL : (ns, Event.t * string) EventEmitter.Event.t
      | Ready : (ns, unit) EventEmitter.Event.t
      | SelectClientCertificate : (ns, Event.t * WebContents.t * string * Certificate.t list * (Certificate.t -> unit)) EventEmitter.Event.t
      | Quit : (ns, Event.t * int) EventEmitter.Event.t
      | WebContentsCreated : (ns, Event.t * WebContents.t) EventEmitter.Event.t
      | WillFinishLaunching : (ns, unit) EventEmitter.Event.t
      | WillQuit : (ns, Event.t) EventEmitter.Event.t
      | WindowAllClosed : (ns, unit) EventEmitter.Event.t

    module PathName = struct
      type t = ..
      type t +=
        | AppData
        | Desktop
        | Documents
        | Downloads
        | Exe
        | Home
        | Module
        | Music
        | PepperFlashSystemPlugin
        | Pictures
        | Temp
        | UserData
        | Videos
    end

    class type t = object
      inherit [ns] EventEmitter.t
      method commandLine : CommandLine.t Js.t
      method dock : Dock.t Js.t
      method addRecentDocument : string -> unit
      method clearRecentdocuments : unit -> unit
      method disableHardwareAcceleration : unit -> unit
      method exit : int -> unit
      method focus : unit -> unit
      method getAppPath : unit -> string
      method getCurrentActivityType : unit -> string
      method getLocale : unit -> string
      method getLoginItemSettings : unit -> LoginItemSettings.t
      method getName : unit -> string
      method getPath : PathName.t -> string
      method getVersion : unit -> string
      method hide : unit -> unit
      method importCertificate : ImportCertificateOptions.t -> (int -> unit) -> unit
      method isAccessibilitySupportEnabled : unit -> bool
      method isDefaultProtocolClient : string -> bool
      method isUnityRunning : unit -> bool
      method makeSingleInstance : (string array -> string -> unit) -> bool
      method quit : unit -> unit
      (* method relaunch *)
      method releaseSingleInstance : unit -> unit
      method removeAsDefaultProtocolClient : string -> unit
      method setAppUserModelId : string -> unit
      method setAsDefaultProtocolClient : string -> unit
      method setLoginItemSettings : LoginItemSettings.t -> unit
      method setName : string -> unit
      method setPath : PathName.t -> string -> unit
      method setUserActivity : string -> 'a Js.t -> (* FIXME *) string ->  unit
      method setUserTasks : Task.t array -> unit
    end [@bs]
  end
  module NativeImage = struct
    class type t = object
      method createFromPath : string -> NativeImage.t Js.t
    end [@bs]
  end
  external app : App.t Js.t = "" [@@bs.module "electron"]
  external nativeImage : NativeImage.t Js.t = "" [@@bs.module "electron"]
  module BrowserWindow = struct
    external mk : unit -> BrowserWindow.t Js.t = "BrowserWindow" [@@bs.new] [@@bs.module "electron"]
  end

  class type remote = object
    method dialog : Dialog.t Js.t
  end [@bs]

  external remote : remote Js.t = "" [@@bs.module "electron"]
end
