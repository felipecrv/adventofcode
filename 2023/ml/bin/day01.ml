open Base

let int_of_char c = Char.to_int c - Char.to_int '0'

module type Parser = sig
  val number : char list -> int option * char list
end

module SimpleParser : Parser = struct
  let number = function
    | ('1' .. '9' as c) :: rest -> Some (int_of_char c), rest
    | _ :: rest -> None, rest
    | [] -> None, []
  ;;
end

module FullParser : Parser = struct
  let number = function
    | 'o' :: 'n' :: ('e' :: _ as rest) -> Some 1, rest
    | 't' :: 'w' :: ('o' :: _ as rest) -> Some 2, rest
    | 't' :: 'h' :: 'r' :: 'e' :: ('e' :: _ as rest) -> Some 3, rest
    | 'f' :: 'o' :: 'u' :: 'r' :: rest -> Some 4, rest
    | 'f' :: 'i' :: 'v' :: ('e' :: _ as rest) -> Some 5, rest
    | 's' :: 'i' :: 'x' :: rest -> Some 6, rest
    | 's' :: 'e' :: 'v' :: 'e' :: ('n' :: _ as rest) -> Some 7, rest
    | 'e' :: 'i' :: 'g' :: 'h' :: ('t' :: _ as rest) -> Some 8, rest
    | 'n' :: 'i' :: 'n' :: ('e' :: _ as rest) -> Some 9, rest
    | ('0' .. '9' as c) :: rest -> Some (int_of_char c), rest
    | _ :: rest -> None, rest
    | [] -> None, []
  ;;
end

module Make (P : Parser) = struct
  let rec calibration chars =
    let rec last chars acc =
      match P.number chars with
      | None, [] -> acc
      | Some n, [] -> n
      | None, rest -> last rest acc
      | Some acc', rest -> last rest acc'
    in
    match P.number chars with
    | None, [] -> 0
    | None, rest -> calibration rest
    | Some n, rest -> (10 * n) + last rest n
  ;;

  let run input =
    let sum =
      String.split_lines input
      |> List.fold_left ~init:0 ~f:(fun sum line ->
           sum + (calibration @@ String.to_list @@ line))
    in
    Stdio.print_endline (Int.to_string sum)
  ;;
end

let part1 =
  let module M = Make (SimpleParser) in
  M.run
;;

let part2 =
  let module M = Make (FullParser) in
  M.run
;;
