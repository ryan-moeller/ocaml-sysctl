type ctlval =
| Node
| Int of int
| String of string
| S64 of int64
| Opaque of bytes
| Uint of int
| Long of int
| Ulong of int
| U64 of int64
| U8 of int32
| U16 of int32
| S8 of int32
| S16 of int32
| S32 of int32
| U32 of int32

external nametomib: string -> int array = "caml_sysctl_nametomib"

external get: int array -> ctlval = "caml_sysctl_get"
external set: int array -> ctlval -> unit = "caml_sysctl_set"

let getbyname name =
  let mib = nametomib name in
  get mib

let setbyname name value =
  let mib = nametomib name in
  set mib value
