open Base

type state =
  | Working
  | Broken
  | Unknown

type token =
  | SpringRun of int * state
  | Num of int
  | Sep
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  let working = [%sedlex.regexp? Plus '.'] in
  let broken = [%sedlex.regexp? Plus '#'] in
  match%sedlex buf with
  | working -> SpringRun (String.length (Sedlexing.Latin1.lexeme buf), Working)
  | broken -> SpringRun (String.length (Sedlexing.Latin1.lexeme buf), Broken)
  | '?' -> SpringRun (1, Unknown)
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | "," -> token buf
  | " " | "\n" -> Sep
  | eof -> End
  | _ -> failwith "token"
;;

let parse_rows buf =
  let rec broken_runs acc =
    match token buf with
    | Num num -> broken_runs (num :: acc)
    | Sep -> List.rev acc
    | _ -> failwith "broken_runs"
  in
  let rec runs acc =
    match token buf with
    | SpringRun (k, s) -> runs ((k, s) :: acc)
    | Sep -> List.rev acc
    | End -> []
    | _ -> failwith "runs"
  in
  let rec rows acc =
    let runs = runs [] in
    if List.is_empty runs
    then List.rev acc
    else (
      let broken_runs = broken_runs [] in
      rows ((runs, broken_runs) :: acc))
  in
  rows []
;;

let is_empty xs = if List.is_empty xs then 1 else 0

let is_singleton x xs =
  match xs with
  | [ x' ] when x = x' -> 1
  | _ -> 0
;;

let solve_row (runs, broken_runs) =
  let memo = Hashtbl.Poly.create () in
  let memo_find ~f runs_tl broken_runs_tl =
    let key = List.length runs_tl, List.length broken_runs_tl in
    Hashtbl.Poly.find_or_add memo key ~default:(fun () -> f runs_tl broken_runs_tl)
  in
  let rec match0 acc (runs : (int * state) list) (broken_runs : int list) =
    match runs with
    (* Bases cases: 0 or 1 runs *)
    | [] | (_, Working) :: [] -> acc + is_empty broken_runs
    | (k, Broken) :: [] -> acc + is_singleton k broken_runs
    | (_, Unknown) :: [] ->
      acc + Int.max (is_empty broken_runs) (is_singleton 1 broken_runs)
    (* Inductive cases: 2 or more runs *)
    | run0 :: runs' ->
      (match run0 with
       | _, Broken -> match_expecting_broken_memo acc runs 0 broken_runs
       | _, Working -> match0 acc runs' broken_runs
       | _, Unknown ->
         let acc' = match_expecting_broken_memo acc runs 0 broken_runs in
         match0 acc' runs' broken_runs)
  and match_expecting_broken0 acc runs broken_runs =
    match runs with
    | [] -> failwith "match_expecting_broken0: expected at least 1 run"
    | (k, Broken) :: [] | (k, Unknown) :: [] -> acc + is_singleton k broken_runs
    | (_, Working) :: _ -> acc
    | (k, Broken) :: runs' | (k, Unknown) :: runs' ->
      (match broken_runs with
       | [] -> acc
       | broken_run0 :: broken_runs' ->
         if broken_run0 = k
         then match_expecting_working acc runs' broken_runs'
         else if k < broken_run0
         then match_expecting_broken_memo acc runs' (broken_run0 - k) broken_runs'
         else acc)
  and match_expecting_broken_memo acc runs broken_run0 broken_runs =
    if broken_run0 = 0
    then acc + memo_find ~f:(match_expecting_broken0 0) runs broken_runs
    else match_expecting_broken0 acc runs (broken_run0 :: broken_runs)
  and match_expecting_working acc runs broken_runs =
    match runs with
    | [] -> failwith "match_expecting_working: expected at least 1 run"
    | (_, Working) :: runs' | (_, Unknown) :: runs' -> match0 acc runs' broken_runs
    | (_, Broken) :: _ -> acc
  in
  match0 0 runs broken_runs
;;

let part1 buf =
  let sum = parse_rows buf |> List.map ~f:solve_row |> List.fold_left ~init:0 ~f:( + ) in
  Stdio.printf "%d\n" sum
;;

let part2 buf =
  let unfold_row (runs, broken_runs) =
    let runs' =
      List.concat
        [ runs
        ; [ 1, Unknown ]
        ; runs
        ; [ 1, Unknown ]
        ; runs
        ; [ 1, Unknown ]
        ; runs
        ; [ 1, Unknown ]
        ; runs
        ]
    in
    let broken_runs' =
      List.concat [ broken_runs; broken_runs; broken_runs; broken_runs; broken_runs ]
    in
    runs', broken_runs'
  in
  let sum =
    parse_rows buf
    |> List.map ~f:(fun row -> solve_row @@ unfold_row @@ row)
    |> List.fold_left ~init:0 ~f:( + )
  in
  Stdio.printf "%d\n" sum
;;
