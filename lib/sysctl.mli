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

val string_of_ctlval : ctlval -> string
val ctltype : ctlfmt -> int
val is_rd : ctlfmt -> bool
val is_wr : ctlfmt -> bool
val is_rw : ctlfmt -> bool
val is_dormant : ctlfmt -> bool
val is_anybody : ctlfmt -> bool
val is_secure : ctlfmt -> bool
val is_prison : ctlfmt -> bool
val is_dyn : ctlfmt -> bool
val is_skip : ctlfmt -> bool
val securelevel : ctlfmt -> int
val is_tunable : ctlfmt -> bool
val is_mpsafe : ctlfmt -> bool
val is_vnet : ctlfmt -> bool
val is_dying : ctlfmt -> bool
val is_caprd : ctlfmt -> bool
val is_capwr : ctlfmt -> bool
val is_caprw : ctlfmt -> bool
val is_stats : ctlfmt -> bool
val is_nofetch : ctlfmt -> bool

val nametomib : string -> ctlmib
val next : ctlmib -> ctlmib option
val next_noskip : ctlmib -> ctlmib option
val name : ctlmib -> string
val format : ctlmib -> ctlfmt
val description : ctlmib -> string option
val get : ctlmib -> ctlval
val set : ctlmib -> ctlval -> unit
val getbyname : string -> ctlval
val setbyname : string -> ctlval -> unit
val iter : ctlmib -> ctlmib Seq.t
val all : ctlmib Seq.t
val iter_noskip : ctlmib -> ctlmib Seq.t
val all_noskip : ctlmib Seq.t
