open Base

type token =
  | Game
  | Num of int
  | Color of color
  | Semicolon
  | End

and color =
  | Red
  | Green
  | Blue

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  match%sedlex buf with
  | "Game " -> Game
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | ": " | ", " | " " -> token buf
  | "red" -> Color Red
  | "green" -> Color Green
  | "blue" -> Color Blue
  | "; " -> Semicolon
  | "\n" | eof -> End
  | _ -> failwith "unexpected input"
;;

type game = int * lower_bounds

and lower_bounds =
  { red : int
  ; green : int
  ; blue : int
  }

let parse_game buf =
  let rec lower_bounds acc =
    match token buf with
    | Num n ->
      (match token buf with
       | Color color ->
         lower_bounds
           (match color with
            | Red -> { acc with red = Int.max acc.red n }
            | Green -> { acc with green = Int.max acc.green n }
            | Blue -> { acc with blue = Int.max acc.blue n })
       | _ -> failwith "expected color count")
    | Semicolon -> lower_bounds acc
    | End -> acc
    | _ -> failwith "expected color count|;|eol|eof"
  in
  match token buf with
  | Game ->
    (match token buf with
     | Num n -> Some (n, lower_bounds { red = 0; green = 0; blue = 0 })
     | _ -> failwith "expected game id")
  | End -> None
  | _ -> failwith "expected game"
;;

let run ~weight buf =
  let open Int64 in
  let rec loop acc =
    match parse_game buf with
    | Some game ->
      (* let id, { red; green; blue } = game in *)
      (* Stdio.printf "Game %d: %d red, %d green, %d blue\n" id red green blue; *)
      loop (acc + Int.to_int64 (weight game))
    | None -> acc
  in
  Stdio.printf "%Ld\n" (loop zero)
;;

let part1 =
  run ~weight:(function id, { red; green; blue } ->
    if red <= 12 && green <= 13 && blue <= 14 then id else 0)
;;

let part2 =
  let one_or = function
    | 0 -> 1
    | i -> i
  in
  run ~weight:(function _, { red; green; blue } ->
    one_or red * one_or green * one_or blue)
;;
