open Base

type number = int * pos
and symbol = info * pos

(** line * col0 * col1 *)
and pos = int * int * int

and info =
  { mutable count : int (* number of nearby part numbers *)
  ; mutable prod : int (* product so far *)
  }

type token =
  | Num of number
  | Sym of symbol
  | End

let pos buf =
  let pos0, pos1 = Sedlexing.lexing_positions buf in
  pos0.pos_lnum - 1, pos0.pos_cnum - pos0.pos_bol, pos1.pos_cnum - pos1.pos_bol
;;

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  let character = [%sedlex.regexp? any] in
  match%sedlex buf with
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf), pos buf)
  | "." | "\n" -> token buf
  | "*" -> Sym ({ count = 0; prod = 1 }, pos buf)
  | character -> Sym ({ count = 0; prod = 0 }, pos buf)
  | eof -> End
  | _ -> assert false
;;

let sums buf =
  let parse =
    let rec aux symbols numbers =
      let tok = token buf in
      match tok with
      | Num num -> aux symbols (num :: numbers)
      | Sym sym -> aux (sym :: symbols) numbers
      | End -> symbols, numbers
    in
    let ss, ns = aux [] [] in
    List.rev ss, List.rev ns
  in
  (* [find_nearby_symbol num symbols] finds the symbol that is nearby the number [num] *)
  let rec find_nearby_symbol = function
    | (n, (line, col0, col1)) as num ->
      (function
       | [] -> None
       | (info, (sline, scol, _)) :: ss ->
         if sline >= line - 1 && sline <= line + 1 && scol >= col0 - 1 && scol < col1 + 1
         then (
           (* calculate count and prod on symbols *)
           info.count <- info.count + 1;
           info.prod <- info.prod * n;
           Some (info, (sline, scol)))
         else find_nearby_symbol num ss)
  in
  let symbols, numbers = parse in
  let rec loop num_sum prod_sum = function
    | [] -> num_sum, prod_sum
    | ((n, _) as num) :: numbers ->
      (match find_nearby_symbol num symbols with
       | None -> loop num_sum prod_sum numbers
       | Some (info, _) ->
         let prod = if info.count = 2 then info.prod else 0 in
         loop (num_sum + n) (prod_sum + prod) numbers)
  in
  loop 0 0 numbers
;;

let part1 buf =
  let sum, _ = sums buf in
  Stdio.printf "%d\n" sum
;;

let part2 buf =
  let _, sum = sums buf in
  Stdio.printf "%d\n" sum
;;
