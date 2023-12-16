open Base

let parse_row s = String.split_on_chars ~on:[ ',' ] @@ String.rstrip @@ s
let hash s = String.fold s ~init:0 ~f:(fun h c -> 17 * (h + Char.to_int c) % 256)

let part1 input =
  let sum = parse_row input |> List.fold ~init:0 ~f:(fun acc s -> acc + hash s) in
  Stdio.printf "%d\n" sum
;;

type inst =
  | Remove of int * string
  | Update of int * string * int

type box = (string * int) list

let parse_inst s =
  if String.contains s '='
  then (
    match String.split_on_chars s ~on:[ '=' ] with
    | [ lbl; v ] -> Update (hash lbl, lbl, Int.of_string v)
    | _ -> failwith "bad update")
  else (
    match String.split_on_chars s ~on:[ '-' ] with
    | lbl :: _ -> Remove (hash lbl, lbl)
    | _ -> failwith "bad remove")
;;

let apply_inst boxes =
  let update (box : box) lbl f =
    let rec aux acc = function
      | [] -> List.rev ((lbl, f) :: acc)
      | (lbl', _) :: xs when String.equal lbl' lbl ->
        List.concat [ List.rev ((lbl, f) :: acc); xs ]
      | x :: xs -> aux (x :: acc) xs
    in
    aux [] box
  in
  function
  | Update (h, lbl, f) ->
    let box' = update boxes.(h) lbl f in
    Array.set boxes h box'
  | Remove (h, lbl) ->
    let box' = List.Assoc.remove boxes.(h) lbl ~equal:String.equal in
    Array.set boxes h box'
;;

let focusing_power box_num box =
  let rec aux acc slot = function
    | [] -> acc
    | (_, f) :: xs ->
      let acc' = acc + ((box_num + 1) * (slot + 1) * f) in
      aux acc' (slot + 1) xs
  in
  aux 0 0 box
;;

let part2 input =
  let boxes = Array.create ~len:256 [] in
  parse_row input |> List.iter ~f:(fun s -> apply_inst boxes @@ parse_inst s);
  let sum = Array.foldi boxes ~init:0 ~f:(fun i acc box -> acc + focusing_power i box) in
  Stdio.printf "%d\n" sum
;;
