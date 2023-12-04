open Base

type token =
  | Card
  | Num of int
  | Sep
  | Newline
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  let character = [%sedlex.regexp? any] in
  match%sedlex buf with
  | "Card " -> Card
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | ": " | " " -> token buf
  | "| " -> Sep
  | "\n" -> Newline
  | eof -> End
  | _ -> assert false
;;

let parse_all_cards buf =
  let rec numbers acc =
    match token buf with
    | Num num -> numbers (num :: acc)
    | _ -> List.rev acc
  in
  let rec loop acc =
    match token buf with
    | Card ->
      (match token buf with
       | Num num ->
         let expected = numbers [] in
         let actual = numbers [] in
         loop ((num, expected, actual) :: acc)
       | _ -> assert false)
    | End -> List.rev acc
    | _ -> assert false
  in
  loop []
;;

let card_score = function
  | _, expected, actual ->
    let rec loop score actual' =
      match actual' with
      | [] -> score
      | x :: xs ->
        let score' =
          if List.mem expected x ~equal:Int.equal
          then (
            let count, sum = score in
            count + 1, if sum > 0 then 2 * sum else 1)
          else score
        in
        loop score' xs
    in
    loop (0, 0) actual
;;

let part1 buf =
  let sum_of_points =
    parse_all_cards buf
    |> List.fold ~init:0 ~f:(fun acc card -> acc + snd (card_score card))
  in
  Stdio.printf "%d\n" sum_of_points
;;

let part2 buf =
  let counts =
    parse_all_cards buf
    |> List.map ~f:(function (cnum, _, _) as card -> cnum, fst (card_score card))
  in
  let n = List.length counts in
  (* memoization table for [solve ((cnum,_)::[])] keyed on [cnum-1] *)
  let memo = Array.create ~len:n None in
  let memo_get i compute =
    match memo.(i) with
    | Some res -> res
    | None ->
      let res = compute () in
      memo.(i) <- Some res;
      res
  in
  let rec solve copies =
    let compute cnum count rest_of_copies () =
      if count = 0
      then solve rest_of_copies
      else (
        let copies' = counts |> List.sub ~pos:cnum ~len:count |> List.rev in
        List.length copies' + solve copies' + solve rest_of_copies)
    in
    match copies with
    | [] -> 0
    | (cnum, count) :: [] -> memo_get (cnum - 1) (compute cnum count [])
    | (cnum, count) :: rest -> compute cnum count rest ()
  in
  let number_of_cards_issued =
    (* build memo table from the end of the list -- Bottom-up Dynamic Programming *)
    List.rev counts
    |> List.fold_left ~init:0 ~f:(fun acc card -> acc + 1 + solve [ card ])
  in
  Stdio.printf "%d\n" number_of_cards_issued
;;
