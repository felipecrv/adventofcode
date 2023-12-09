open Base

type token =
  | Num of int
  | Newline
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Opt "-", Plus digit] in
  match%sedlex buf with
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | " " -> token buf
  | "\n" -> Newline
  | eof -> End
  | _ -> failwith "token"
;;

let parse_rows buf =
  let rec numbers_row acc =
    match token buf with
    | Num num -> numbers_row (num :: acc)
    | Newline -> List.rev acc
    | End -> []
  in
  let rec aux acc =
    let row = numbers_row [] in
    if List.is_empty row then List.rev acc else aux (row :: acc)
  in
  aux []
;;

let solve row ~backwards =
  let rec all_the_same_aux xs = function
    | false -> false
    | true ->
      (match xs with
       | [] | [ _ ] -> true
       | x :: (x' :: _ as rest) -> all_the_same_aux rest (x = x'))
  in
  let rec diffs_aux acc = function
    | [] | _ :: [] -> List.rev acc
    | x :: (x' :: _ as rest) -> diffs_aux ((x' - x) :: acc) rest
  in
  let rec decompose_aux acc row =
    if all_the_same_aux row true
    then acc
    else (
      let row' = diffs_aux [] row in
      decompose_aux (row' :: acc) row')
  in
  let rec aux delta = function
    | row :: rows ->
      let delta' =
        if backwards then List.hd_exn row - delta else delta + List.last_exn row
      in
      aux delta' rows
    | [] -> delta
  in
  aux 0 (decompose_aux [ row ] row)
;;

let part1 buf =
  let sum =
    parse_rows buf
    |> List.map ~f:(solve ~backwards:false)
    |> List.fold_left ~init:0 ~f:( + )
  in
  Stdio.printf "%d\n" sum
;;

let part2 buf =
  let sum =
    parse_rows buf
    |> List.map ~f:(solve ~backwards:true)
    |> List.fold_left ~init:0 ~f:( + )
  in
  Stdio.printf "%d\n" sum
;;
