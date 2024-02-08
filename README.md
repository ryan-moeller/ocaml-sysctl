Usage
-----

```ocaml
# let mib = Sysctl.nametomib "vfs.zfs.fletcher_4_impl";;
val mib : int array = [|3; 222; 2147481069|]
# Sysctl.get mib;;
- : Sysctl.ctlval =
Sysctl.String "[fastest] scalar superscalar superscalar4 sse2 ssse3 avx2 "
# Sysctl.set mib (Sysctl.String "avx2");;
- : unit = ()
# Sysctl.getbyname "vfs.zfs.fletcher_4_impl";;
- : Sysctl.ctlval =
Sysctl.String "fastest scalar superscalar superscalar4 sse2 ssse3 [avx2] "
# Sysctl.setbyname "vfs.zfs.fletcher_4_impl" (Sysctl.String "fastest");;
- : unit = ()
# Sysctl.format @@ Sysctl.nametomib "vfs.zfs.abd_scatter_min_size";;
- : int * string = (3222016008, "LU")
# Sysctl.description @@ Sysctl.nametomib "vfs.zfs.blake3_impl";;
- : string = "Select BLAKE3 implementation."
# Sysctl.all |> Seq.map Sysctl.name |> Seq.take 20 |> List.of_seq;;
- : string list =
["sysctl"; "sysctl.name"; "sysctl.next"; "sysctl.name2oid"; "sysctl.oidfmt";
 "sysctl.oiddescr"; "sysctl.oidlabel"; "sysctl.nextnoskip"; "kern.ostype";
 "kern.osrelease"; "kern.osrevision"; "kern.version"; "kern.maxvnodes";
 "kern.maxproc"; "kern.maxfiles"; "kern.argmax"; "kern.securelevel";
 "kern.hostname"; "kern.hostid"; "kern.clockrate"]
```
