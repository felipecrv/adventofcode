open Base

type color =
  | Red
  | Green
  | Blue

type token =
  | Game
  | Num of int
  | Color of color
  | Semicolon
  | EndOfLine
  | EndOfFile

let digit = [%sedlex.regexp? '0' .. '9']
let number = [%sedlex.regexp? Plus digit]

let fail expected got after =
  (match after with
   | Some after ->
     Printf.sprintf "Expected `%s` after `%s`, but got `%s`" expected after got
   | None -> Printf.sprintf "Expected `%s` but got `%s`" expected got)
  |> failwith
;;

let fail_at buf =
  let pos = fst (Sedlexing.lexing_positions buf) in
  Printf.sprintf
    "At line %d, column %d: %s"
    pos.pos_lnum
    (pos.pos_cnum - pos.pos_bol)
    (Sedlexing.Latin1.lexeme buf)
  |> failwith
;;

let rec token buf =
  match%sedlex buf with
  | "Game " -> Game
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | ": " | ", " | " " -> token buf
  | "red" -> Color Red
  | "green" -> Color Green
  | "blue" -> Color Blue
  | "; " -> Semicolon
  | "\n" -> EndOfLine
  | eof -> EndOfFile
  | _ -> fail_at buf
;;

type game = int * stats list

and stats =
  { red : int option
  ; green : int option
  ; blue : int option
  }

let empty = { red = None; green = None; blue = None }

let parse_game buf =
  let add_color color (k : int) = function
    | s :: rest ->
      let s' =
        match color with
        | Red -> { s with red = Some k }
        | Green -> { s with green = Some k }
        | Blue -> { s with blue = Some k }
      in
      s' :: rest
    | [] -> failwith "Empty stats list"
  in
  let rec parse_stats buf acc =
    match token buf with
    | Num n ->
      (match token buf with
       | Color color -> parse_stats buf (add_color color n acc)
       | _ -> fail "red|green|blue" "" (Some (Int.to_string n ^ " ")))
    | Semicolon -> parse_stats buf (empty :: acc)
    | EndOfLine | EndOfFile -> acc
    | _ -> fail "red|green|blue|;" "" None
  in
  match token buf with
  | Game ->
    (match token buf with
     | Num n ->
       let game = n, parse_stats buf [ empty ] in
       Some game
     | _ -> fail "[0-9]+: " "" (Some "Game "))
  | EndOfFile -> None
  | _ -> fail "Game " "" None
;;

let string_of_lower_bound s = function
  | None -> s ^ ":?"
  | Some n -> s ^ ":" ^ Int.to_string n
;;

let rec print_stats game is_tail =
  match game with
  | s :: rest ->
    Stdio.printf
      "%s%s, %s, %s"
      (if is_tail then "; " else "")
      (string_of_lower_bound "red" s.red)
      (string_of_lower_bound "green" s.green)
      (string_of_lower_bound "blue" s.blue);
    print_stats rest true
  | [] -> Stdio.print_endline ""
;;

let print_game game =
  Stdio.printf "Game %d: " (fst game);
  print_stats (snd game) false
;;

let reduce_lower_bounds stats =
  List.fold_left
    ~init:{ red = None; green = None; blue = None }
    ~f:(fun acc s ->
      let max = function
        | None -> fun rhs -> rhs
        | Some lhs ->
          (function
           | Some rhs -> Some (Int.max lhs rhs)
           | None -> Some lhs)
      in
      { red = max acc.red s.red
      ; green = max acc.green s.green
      ; blue = max acc.blue s.blue
      })
    stats
;;

let is_possible = function
  | red, green, blue ->
    let aux upper_bound = function
      | None -> true
      | Some n -> n <= upper_bound
    in
    fun s -> aux red s.red && aux green s.green && aux blue s.blue
;;

let power = function
  | { red; green; blue } ->
    [ red; green; blue ]
    |> List.fold_left ~init:(Int.to_int64 1) ~f:(fun acc -> function
         | Some n ->
           let open Int64 in
           acc * Int.to_int64 n
         | None -> acc)
;;

let part1 input =
  let rec loop buf sum power_sum =
    match parse_game buf with
    | Some game ->
      print_game game;
      let id, stats = game in
      let lower_bounds = reduce_lower_bounds stats in
      if is_possible (12, 13, 14) lower_bounds
      then (
        Stdio.printf "Game %d is possible\n" id;
        print_stats [ lower_bounds ] false;
        loop
          buf
          (sum + id)
          (let open Int64 in power_sum + power lower_bounds))
      else loop buf sum (let open Int64 in power_sum + power lower_bounds)
    | None -> sum, power_sum
  in
  let sum, power_sum = loop (Sedlexing.Latin1.from_channel input) 0 Int64.zero in
  Stdio.printf "%d\n" sum;
  Stdio.printf "%Ld\n" power_sum
;;

let part2 input =
  ignore input;
  Stdio.print_endline "Not implemented"
;;
