let () =
  Sysctl.nametomib "vm"
  |> Sysctl.iter_noskip
  |> Seq.map (fun mib ->
         (Sysctl.name mib, Sysctl.format mib, Sysctl.description mib))
  |> Seq.map (fun (name, (_kind, fmt), desc) ->
         match desc with
         | Some descr -> (name, fmt, descr)
         | None -> (name, fmt, ""))
  |> Seq.iter (fun (name, fmt, desc) ->
         Printf.printf "%s [%s]: %s\n" name fmt desc)

let () =
  Sysctl.nametomib "vm" |> Sysctl.iter
  |> Seq.map (fun mib ->
         ( Sysctl.name mib,
           Sysctl.format mib,
           Sysctl.description mib,
           Sysctl.get mib ))
  |> Seq.map (fun (name, (_kind, fmt), desc, value) ->
         let v = Sysctl.string_of_ctlval value in
         match desc with
         | Some descr -> (name, fmt, descr, v)
         | None -> (name, fmt, "", v))
  |> Seq.iter (fun (name, fmt, desc, value) ->
         Printf.printf "%s [%s]: %s = %s\n" name fmt desc value)

let () = Sysctl.all |> Seq.length |> Printf.printf "all: %d mibs\n"

let () =
  Sysctl.all_noskip |> Seq.length |> Printf.printf "all_noskip: %d mibs\n"
