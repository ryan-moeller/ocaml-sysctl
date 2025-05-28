(lang dune 3.7)

(name sysctl)

(generate_opam_files true)

(source
 (github ryan-moeller/ocaml-sysctl))

(authors "Ryan Moeller")

(maintainers "Ryan Moeller")

(license LICENSE)

(documentation https://url/to/documentation)

(package
 (name sysctl)
 (synopsis "Bindings for FreeBSD sysctl(3) functions")
 (description "Functions to get and set FreeBSD sysctls")
 (depends ocaml dune)
 (tags
  (topics FreeBSD sysctl)))

; See the complete stanza docs at https://dune.readthedocs.io/en/stable/dune-files.html#dune-project
