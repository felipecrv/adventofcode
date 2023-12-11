open Base
open Core
module Sys = Stdlib.Sys

module type Problem = sig
  val part1 : Stdlib.in_channel -> unit
  val part2 : Stdlib.in_channel -> unit
end

module type StringInputProblem = sig
  val part1 : string -> unit
  val part2 : string -> unit
end

module type SedlexInputProblem = sig
  val part1 : Sedlexing.lexbuf -> unit
  val part2 : Sedlexing.lexbuf -> unit
end

module StringInput (S : StringInputProblem) : Problem = struct
  let part1 in_channel =
    let input = In_channel.input_all in_channel in
    S.part1 input
  ;;

  let part2 in_channel =
    let input = In_channel.input_all in_channel in
    S.part2 input
  ;;
end

module SedlexInput (S : SedlexInputProblem) : Problem = struct
  let part1 in_channel =
    let lexbuf = Sedlexing.Latin1.from_channel in_channel in
    S.part1 lexbuf
  ;;

  let part2 in_channel =
    let lexbuf = Sedlexing.Latin1.from_channel in_channel in
    S.part2 lexbuf
  ;;
end

module Private = struct
  let problems : (int * (module Problem)) list ref = ref []
  let get_problem ~day = List.Assoc.find !problems day ~equal:Int.equal

  let register_problem ~day problem =
    match get_problem ~day with
    | Some _ -> failwith "Problem already registered"
    | None -> problems := (day, problem) :: !problems
  ;;

  type input_spec =
    | Part1 of string
    | Part2 of string

  (** [Silver test] means that the input file is only used for part 1 and is a
      small test file if test is true. *)
  type compatibility =
    | Both of bool
    | Silver of bool (* Part 1 only *)
    | Gold of bool (* Part 2 only *)

  let make_path sday basename = sday ^ "/" ^ basename

  let input_candidates =
    [ "test_in", Both true
    ; "test_in0", Both true
    ; "test_in1", Both true
    ; "test_in2", Both true
    ; "test_in_silver", Silver true
    ; "test_in_silver0", Silver true
    ; "test_in_silver1", Silver true
    ; "test_in_silver2", Silver true
    ; "test_in_gold", Gold true
    ; "test_in_gold0", Gold true
    ; "test_in_gold1", Gold true
    ; "test_in_gold2", Gold true
    ; "in", Both false
    ; "in0", Both false
    ; "in1", Both false
    ; "in2", Both false
    ; "in_silver", Silver false
    ; "in_silver0", Silver false
    ; "in_silver1", Silver false
    ; "in_silver2", Silver false
    ; "in_gold", Gold false
    ; "in_gold0", Gold false
    ; "in_gold1", Gold false
    ; "in_gold2", Gold false
    ]
  ;;

  let match_compatibility ~part ~test_only candidate =
    let _, compatibility = candidate in
    match compatibility with
    | Silver t -> Option.mem ~equal:Int.equal part 1 && ((not test_only) || t)
    | Gold t -> Option.mem ~equal:Int.equal part 2 && ((not test_only) || t)
    | Both t -> (not test_only) || t
  ;;

  let filtered_candidates part test_only =
    List.filter input_candidates ~f:(match_compatibility ~part ~test_only)
  ;;

  let input_schedule sday part test_only =
    let schedule = function
      | basename, compatibility ->
        let path = make_path sday basename in
        if Sys.file_exists path
        then (
          match part with
          | None ->
            (match compatibility with
             | Both _ -> [ Part1 path; Part2 path ]
             | Silver _ -> [ Part1 path ]
             | Gold _ -> [ Part2 path ])
          | Some 1 ->
            (match compatibility with
             | Both _ -> [ Part1 path ]
             | Silver _ -> [ Part1 path ]
             | Gold _ -> [])
          | Some 2 ->
            (match compatibility with
             | Both _ -> [ Part2 path ]
             | Silver _ -> []
             | Gold _ -> [ Part2 path ])
          | Some _ -> failwith "Invalid problem part: must be 1 or 2.")
        else []
    in
    input_candidates
    |> List.filter ~f:(match_compatibility ~part ~test_only)
    |> List.map ~f:schedule
    |> List.concat
  ;;

  let run_problem ~day ~part ~test_only =
    let string_of_day d = (if d < 10 then "day0" else "day") ^ Int.to_string d in
    match get_problem ~day with
    | None ->
      Stdio.Out_channel.fprintf
        Stdio.stderr
        "Problem for day %d not registered in bin/main.ml\n"
        day
    | Some (module Problem) ->
      let sday = string_of_day day in
      if not (Sys.file_exists sday) then Sys.mkdir sday 0o755;
      let schedule = input_schedule sday part test_only in
      if List.is_empty schedule
      then (
        Stdio.printf "No input files found in %s/. Looked for one of these files:\n" sday;
        input_candidates
        |> List.filter ~f:(match_compatibility ~part ~test_only)
        |> List.iter ~f:(fun (basename, _) ->
             Stdio.printf "  %s\n" (make_path sday basename)))
      else
        List.iter schedule ~f:(fun input_spec ->
          let star, name, run_part, path =
            match input_spec with
            | Part1 path -> "⭐", "part1", Problem.part1, path
            | Part2 path -> "🌟", "part2", Problem.part2, path
          in
          Stdio.printf "%s Running %s of %s on %s\n" star name sday path;
          let input_channel = In_channel.create path in
          Stdio.Out_channel.flush Stdio.stdout;
          run_part input_channel)
  ;;

  open Cmdliner

  let day =
    let doc = {|Run problem number "day" $(docv).|} in
    Arg.(value & opt (some int) None & info [ "day" ] ~docv:"DAY" ~doc)
  ;;

  let part =
    let doc = "Run problem part $(docv)." in
    Arg.(value & opt (some int) None & info [ "part" ] ~docv:"PART" ~doc)
  ;;

  let test_only =
    let doc = "Run only the small test inputs." in
    Arg.(value & flag & info [ "test" ] ~doc)
  ;;

  let main () =
    let run (day : int option) (part : int option) (test_only : bool) : unit Term.ret =
      match day, part with
      | None, _ -> `Error (false, {|"day" argument required.|})
      | Some day, (None | Some (1 | 2)) ->
        run_problem ~day ~part ~test_only;
        `Ok ()
      | _, Some _ -> `Error (false, {|"part" argument must be 1 or 2.|})
    in
    let info = Cmd.info "aoc" ~doc:"Run Advent of Code problem." in
    let cmd = Cmd.v info Term.(ret (const run $ day $ part $ test_only)) in
    exit @@ Cmd.eval cmd
  ;;
end

let register = Private.register_problem
let main = Private.main
