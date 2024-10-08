type ctlmib = int array
type ctlfmt = int * string

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

let string_of_ctlval = function
  | Node -> "<node>"
  | Opaque _ -> "<bytes>"
  | String s -> s
  | Int x | Uint x | Long x | Ulong x -> string_of_int x
  | S8 x | U8 x | S16 x | U16 x | S32 x | U32 x -> Int32.to_string x
  | S64 x | U64 x -> Int64.to_string x

(* Helpers for dealing with format information *)
let ctltype (kind, _fmt) = kind land 0xf
let is_rd (kind, _fmt) = kind land 0x80000000 != 0
let is_wr (kind, _fmt) = kind land 0x40000000 != 0
let is_rw kf = is_rd kf && is_wr kf
let is_dormant (kind, _fmt) = kind land 0x20000000 != 0
let is_anybody (kind, _fmt) = kind land 0x10000000 != 0
let is_secure (kind, _fmt) = kind land 0x08000000 != 0
let is_prison (kind, _fmt) = kind land 0x04000000 != 0
let is_dyn (kind, _fmt) = kind land 0x02000000 != 0
let is_skip (kind, _fmt) = kind land 0x01000000 != 0
let securelevel (kind, _fmt) = (kind lsr 20) land 0xf
let is_tunable (kind, _fmt) = kind land 0x00080000 != 0
let is_mpsafe (kind, _fmt) = kind land 0x00040000 != 0
let is_vnet (kind, _fmt) = kind land 0x00020000 != 0
let is_dying (kind, _fmt) = kind land 0x00010000 != 0
let is_caprd (kind, _fmt) = kind land 0x00008000 != 0
let is_capwr (kind, _fmt) = kind land 0x00004000 != 0
let is_caprw kf = is_caprd kf && is_capwr kf
let is_stats (kind, _fmt) = kind land 0x00002000 != 0
let is_nofetch (kind, _fmt) = kind land 0x00001000 != 0

external nametomib : string -> ctlmib = "caml_sysctl_nametomib"
external next : ctlmib -> ctlmib option = "caml_sysctl_next"
external next_noskip : ctlmib -> ctlmib option = "caml_sysctl_next_noskip"
external name : ctlmib -> string = "caml_sysctl_name"
external format : ctlmib -> ctlfmt = "caml_sysctl_format"
external description : ctlmib -> string option = "caml_sysctl_description"
external get : ctlmib -> ctlval = "caml_sysctl_get"
external set : ctlmib -> ctlval -> unit = "caml_sysctl_set"

let getbyname name =
  let mib = nametomib name in
  get mib

let setbyname name value =
  let mib = nametomib name in
  set mib value

let iter mib =
  let prefix_match nxt =
    let len = Array.length mib in
    if len > Array.length nxt then false
    else
      let prefix = Array.sub nxt 0 len in
      prefix = mib
  in
  let rec iter1 mib () =
    match next mib with
    | Some nxt when prefix_match nxt -> Seq.Cons (mib, iter1 nxt)
    | _ -> Seq.Cons (mib, Seq.empty)
  in
  iter1 mib

let all =
  let rec iter1 mib () =
    match next mib with
    | Some nxt -> Seq.Cons (mib, iter1 nxt)
    | None -> Seq.Cons (mib, Seq.empty)
  in
  iter1 [| 1 |]

let iter_noskip mib =
  let prefix_match nxt =
    let len = Array.length mib in
    if len > Array.length nxt then false
    else
      let prefix = Array.sub nxt 0 len in
      prefix = mib
  in
  let rec iter1 mib () =
    match next_noskip mib with
    | Some nxt when prefix_match nxt -> Seq.Cons (mib, iter1 nxt)
    | _ -> Seq.Cons (mib, Seq.empty)
  in
  iter1 mib

let all_noskip =
  let rec iter1 mib () =
    match next_noskip mib with
    | Some nxt -> Seq.Cons (mib, iter1 nxt)
    | None -> Seq.Cons (mib, Seq.empty)
  in
  iter1 [| 1 |]
