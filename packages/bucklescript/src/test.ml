(*class type t = object
    method m : unit -> unit
    external m2 : unit -> unit = "" [@@bs.val]
end [@bs]

external blah : t Js.t = "" [@@bs.module]

let b = blah##m ()*)