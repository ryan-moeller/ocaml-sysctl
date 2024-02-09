let () =
  Sysctl.nametomib "kstat.zfs"
  |> Sysctl.iter
  |> Seq.map (fun mib ->
         (Sysctl.name mib, Sysctl.format mib, Sysctl.description mib))
  |> Seq.map (fun (name, (_kind, fmt), desc) ->
         match desc with
         | Some descr -> (name, fmt, descr)
         | None -> (name, fmt, ""))
  |> Seq.iter (fun (name, fmt, desc) ->
         Printf.printf "%s [%s]: %s\n" name fmt desc)
