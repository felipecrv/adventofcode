open Base

let count_sorted =
  let rec loop acc count x xs =
    match xs with
    | [] -> count :: acc
    | x' :: xs' -> (if x = x' then loop acc (count + 1) else loop (count :: acc) 1) x' xs'
  in
  function
  | [] -> []
  | x :: xs -> loop [] 1 x xs |> List.rev
;;

type token =
  | Hand of string
  | Bid of string
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let rank = [%sedlex.regexp? 'A' | 'K' | 'Q' | 'J' | 'T' | digit] in
  let hand = [%sedlex.regexp? Rep (rank, 5)] in
  let number_nl = [%sedlex.regexp? Plus digit, '\n'] in
  match%sedlex buf with
  | hand -> Hand (Sedlexing.Latin1.lexeme buf)
  | number_nl -> Bid (String.strip (Sedlexing.Latin1.lexeme buf))
  | " " -> token buf
  | eof -> End
  | _ -> failwith "token"
;;

let parse_hands buf ~joker_rule =
  (* A, K, Q, J, T, 9, 8, 7, 6, 5, 4, 3, 2 *)
  let card_strength = function
    | 'A' -> 12
    | 'K' -> 11
    | 'Q' -> 10
    | 'J' -> if joker_rule then -1 else 9
    | 'T' -> 8
    | c -> Char.to_int c - Char.to_int '0' - 2
  in
  let rec aux acc =
    match token buf with
    | Hand s ->
      let hand = s |> String.to_list |> List.map ~f:card_strength in
      let triplet = s, hand, hand |> List.sort ~compare:Int.compare in
      (match token buf with
       | Bid num -> aux ((triplet, Int.of_string num) :: acc)
       | _ -> failwith "expected bid")
    | End -> List.rev acc
    | Bid _ -> failwith "expected hand"
  in
  aux []
;;

let hand_type s sorted_hand =
  let without_jokers = sorted_hand |> List.filter ~f:(fun x -> x <> -1) in
  let sorted_counts = count_sorted without_jokers |> List.sort ~compare:Int.compare in
  match sorted_counts with
  | [] | [ 1 ] | [ 2 ] | [ 3 ] | [ 4 ] | [ 5 ] -> 6 (* Five of a kind *)
  | [ 1; _ ] -> 5 (* Four of a kind *)
  | [ 2; _ ] -> 4 (* Full house *)
  | [ 1; 1; _ ] -> 3 (* Three of a kind *)
  | [ 1; 2; 2 ] -> 2 (* Two pair *)
  | [ 1; 1; 1; 1 ] | [ 1; 1; 1; 2 ] -> 1 (* One pair *)
  | [ 1; 1; 1; 1; 1 ] -> 0 (* High card *)
  | _ -> failwith ("unrecognized hand: " ^ s)
;;

let compare_hands (sa, a, sorted_a) (sb, b, sorted_b) =
  match Int.compare (hand_type sa sorted_a) (hand_type sb sorted_b) with
  | 0 -> List.compare Int.compare a b
  | cmp -> cmp
;;

let run buf ~joker_rule =
  let sum =
    parse_hands buf ~joker_rule
    |> List.sort ~compare:(fun (a, _) (b, _) -> compare_hands a b)
    |> List.foldi ~init:0 ~f:(fun i acc (_, bid) -> acc + ((i + 1) * bid))
  in
  Stdio.printf "%d\n" sum
;;

let part1 buf = run buf ~joker_rule:false
let part2 buf = run buf ~joker_rule:true
