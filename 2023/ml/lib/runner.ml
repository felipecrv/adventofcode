open Base
open Core
module Sys = Caml.Sys

module type Problem = sig
  val part1 : Caml.in_channel -> unit
  val part2 : Caml.in_channel -> unit
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

  type compatibility =
    | Both
    | Silver (* Part 1 only *)
    | Gold (* Part 2 only *)

  let make_path sday basename = sday ^ "/" ^ basename

  let input_candidates =
    [ "test_in", Both
    ; "test_in0", Both
    ; "test_in1", Both
    ; "test_in2", Both
    ; "test_in_silver", Silver
    ; "test_in_silver0", Silver
    ; "test_in_silver1", Silver
    ; "test_in_silver2", Silver
    ; "test_in_gold", Gold
    ; "test_in_gold0", Gold
    ; "test_in_gold1", Gold
    ; "test_in_gold2", Gold
    ; "in", Both
    ; "in0", Both
    ; "in1", Both
    ; "in2", Both
    ; "in_silver", Silver
    ; "in_silver0", Silver
    ; "in_silver1", Silver
    ; "in_silver2", Silver
    ; "in_gold", Gold
    ; "in_gold0", Gold
    ; "in_gold1", Gold
    ; "in_gold2", Gold
    ]
  ;;

  let filtered_candidates part =
    List.filter input_candidates ~f:(fun (_, compatibility) ->
      match part with
      | Some 1 ->
        (match compatibility with
         | Silver | Both -> true
         | _ -> false)
      | Some 2 ->
        (match compatibility with
         | Gold | Both -> true
         | _ -> false)
      | Some _ | None -> true)
  ;;

  let input_schedule sday =
    let schedule = function
      | basename, compatibility ->
        let path = make_path sday basename in
        if Sys.file_exists path
        then (
          match compatibility with
          | Both -> [ Part1 path; Part2 path ]
          | Silver -> [ Part1 path ]
          | Gold -> [ Part2 path ])
        else []
    in
    input_candidates |> List.map ~f:schedule |> List.concat
  ;;

  let run_problem ~day ~part =
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
      let schedule =
        match part with
        | None -> input_schedule sday
        | Some 1 ->
          input_schedule sday
          |> List.filter ~f:(function
               | Part1 _ -> true
               | _ -> false)
        | Some 2 ->
          input_schedule sday
          |> List.filter ~f:(function
               | Part2 _ -> true
               | _ -> false)
        | Some _ -> failwith "Invalid problem part: must be 1 or 2."
      in
      if List.is_empty schedule
      then (
        Stdio.printf "No input files found in %s/. Looked for one of these files:\n" sday;
        filtered_candidates part
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

  let main () =
    let run (day : int option) (part : int option) : unit Term.ret =
      match day, part with
      | None, _ -> `Error (false, {|"day" argument required.|})
      | Some day, (None | Some (1 | 2)) ->
        run_problem ~day ~part;
        `Ok ()
      | _, Some _ -> `Error (false, {|"part" argument must be 1 or 2.|})
    in
    let info = Cmd.info "aoc" ~doc:"Run Advent of Code problem." in
    let cmd = Cmd.v info Term.(ret (const run $ day $ part)) in
    exit @@ Cmd.eval cmd
  ;;
end

let register = Private.register_problem
let main = Private.main
