open Base

type token =
  | Num of string
  | Newline
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  match%sedlex buf with
  | "Time: " | "Distance: " | " " -> token buf
  | number -> Num (Sedlexing.Latin1.lexeme buf)
  | "\n" -> Newline
  | eof -> End
  | _ -> failwith "token"
;;

let parse_stats buf ~allow_spaces =
  let rec numbers acc =
    match token buf with
    | Num num -> numbers (num :: acc)
    | Newline | End -> List.rev acc
  in
  let aux () =
    if allow_spaces
    then [ numbers [] |> String.concat ~sep:"" |> Int.of_string ]
    else numbers [] |> List.map ~f:Int.of_string
  in
  let times = aux () in
  let dists = aux () in
  List.zip_exn times dists
;;

let run buf ~allow_spaces =
  let num_ways_to_win (time, max_dist) =
    let can_win accel_time =
      let final_speed = accel_time in
      let run_time = time - accel_time in
      let final_dist = final_speed * run_time in
      final_dist > max_dist
    in
    let rec loop accel_time acc =
      if accel_time >= time
      then acc
      else loop (accel_time + 1) (if can_win accel_time then acc + 1 else acc)
    in
    loop 1 0
  in
  let prod =
    parse_stats buf ~allow_spaces
    |> List.fold ~init:1 ~f:(fun acc x -> acc * num_ways_to_win x)
  in
  Stdio.printf "%d\n" prod
;;

let part1 buf = run buf ~allow_spaces:false
let part2 buf = run buf ~allow_spaces:true
