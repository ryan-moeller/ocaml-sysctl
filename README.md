OCaml Bindings for FreeBSD sysctl(3)
====================================

[![13.5-STABLE Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=snapshots/amd64/13.5-STABLE)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)
[![14.3-STABLE Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=snapshots/amd64/14.3-STABLE)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)
[![15.0-STABLE Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=snapshots/amd64/15.0-STABLE)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)
[![13.5-RELEASE Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=releases/amd64/13.5-RELEASE)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)
[![14.3-RELEASE Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=releases/amd64/14.3-RELEASE)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)
[![15.0-RC2 Build Status](https://api.cirrus-ci.com/github/ryan-moeller/ocaml-sysctl.svg?branch=main&task=releases/amd64/15.0-RC2)](https://cirrus-ci.com/github/ryan-moeller/ocaml-sysctl)

Usage
-----

```ocaml
# let mib = Sysctl.nametomib "vfs.zfs.fletcher_4_impl";;
val mib : Sysctl.ctlmib = [|3; 222; 2147481069|]
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
- : Sysctl.ctlfmt = (3222016008, "LU")
# Sysctl.description @@ Sysctl.nametomib "vfs.zfs.blake3_impl";;
- : string option = Some "Select BLAKE3 implementation."
# Sysctl.nametomib "compat" |> Sysctl.iter |> Seq.map Sysctl.name |> Seq.iter print_endline;;
compat.linux32.emulate_i386
compat.linux32.maxvmem
compat.linux32.maxssiz
compat.linux32.maxdsiz
compat.linux.use_real_ifnames
compat.linux.emul_path
compat.linux.oss_version
compat.linux.osrelease
compat.linux.osname
compat.linux.setid_allowed
compat.linux.map_sched_prio
compat.linux.preserve_vstatus
compat.linux.ignore_ip_recverr
compat.linux.dummy_rlimits
compat.linux.default_stacksize
compat.linux.default_openfiles
compat.linux.debug
compat.ia32.maxvmem
compat.ia32.maxssiz
compat.ia32.maxdsiz
compat.linuxkpi.skb.mem_limit
compat.linuxkpi.lkpi_pci_nseg1_fail
compat.linuxkpi.task_struct_reserve
compat.linuxkpi.net_ratelimit
compat.linuxkpi.warn_dump_stack
compat.linuxkpi.rcu_debug
compat.linuxkpi.debug
compat.linuxkpi.80211.debug
compat.linuxkpi.80211.tkip
compat.linuxkpi.80211.hw_crypto
compat.linuxkpi.80211.order_scanlist
- : unit = ()
# Sysctl.all_noskip |> Seq.length;;
- : int = 20977
```
