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
external next: int array -> int array option = "caml_sysctl_next"
external next_noskip: int array -> int array option = "caml_sysctl_next_noskip"
external name: int array -> string = "caml_sysctl_name"
external format: int array -> int * string = "caml_sysctl_format"
external description: int array -> string = "caml_sysctl_description"

external get: int array -> ctlval = "caml_sysctl_get"
external set: int array -> ctlval -> unit = "caml_sysctl_set"

let getbyname name =
  let mib = nametomib name in
  get mib

let setbyname name value =
  let mib = nametomib name in
  set mib value

let iter mib () =
  let prefix_match n =
    let l = Array.length mib in
    let p = Array.sub n 0 l in
    List.equal Int.equal (Array.to_list mib) (Array.to_list p)
  in
  let rec iter1 mib () =
    match next mib with
    | Some(nxt) ->
        if prefix_match nxt
        then Seq.Cons (mib, (iter1 nxt))
        else Seq.Cons (mib, (fun () -> Seq.Nil))
    | None -> Seq.Cons (mib, (fun () -> Seq.Nil))
  in
  iter1 mib ()

let all = iter [||]

let iter_noskip mib () =
  let prefix_match n =
    let l = Array.length mib in
    let p = Array.sub n 0 l in
    List.equal Int.equal (Array.to_list mib) (Array.to_list p)
  in
  let rec iter1 mib () =
    match next_noskip mib with
    | Some(nxt) ->
        if prefix_match nxt
        then Seq.Cons (mib, (iter1 nxt))
        else Seq.Cons (mib, (fun () -> Seq.Nil))
    | None -> Seq.Cons (mib, (fun () -> Seq.Nil))
  in
  iter1 mib ()

let all_noskip = iter_noskip [||]
