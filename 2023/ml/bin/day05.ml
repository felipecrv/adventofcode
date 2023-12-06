open Base

type token =
  | Seeds
  | Num of int
  | MappingOf of string
  | Newline
  | End

let rec token buf =
  let digit = [%sedlex.regexp? '0' .. '9'] in
  let number = [%sedlex.regexp? Plus digit] in
  match%sedlex buf with
  | "seeds: " -> Seeds
  | number -> Num (Int.of_string (Sedlexing.Latin1.lexeme buf))
  | "\n" -> Newline
  | " map:\n" | " " -> token buf
  | "seed-to-soil"
  | "soil-to-fertilizer"
  | "fertilizer-to-water"
  | "water-to-light"
  | "light-to-temperature"
  | "temperature-to-humidity"
  | "humidity-to-location" -> MappingOf (Sedlexing.Latin1.lexeme buf)
  | eof -> End
  | _ -> failwith "token"
;;

type triplet = int * int * int (* des0, src0, len *)

let triplet_compare (_, src0, _) (_, src0', _) =
  let cmp = Int.compare src0 src0' in
  assert (cmp <> 0);
  cmp
;;

let parse_all_input buf =
  let rec numbers_row acc =
    match token buf with
    | Num num -> numbers_row (num :: acc)
    | Newline | End -> List.rev acc
    | _ -> failwith "numbers_row"
  in
  let rec triplets acc =
    match token buf with
    | Num num ->
      let t =
        match numbers_row [ num ] with
        | [ des0; src0; len ] -> des0, src0, len
        | _ -> failwith "invalid triplet"
      in
      triplets (t :: acc)
    | Newline | End -> List.sort acc ~compare:triplet_compare
    | _ -> failwith "triplets"
  in
  let rec all_maps acc =
    match token buf with
    | MappingOf t ->
      let table = triplets [] in
      all_maps ((t, table) :: acc)
    | End -> List.rev acc
    | _ -> failwith "all_maps"
  in
  match token buf with
  | Seeds ->
    let seeds = numbers_row [] in
    (match token buf with
     | Newline -> seeds, all_maps []
     | _ -> failwith "seeds")
  | _ -> failwith "parse_all_input"
;;

let seeds_as_ranges seeds =
  let rec aux acc = function
    | [] -> List.rev acc
    | s0 :: len :: rest -> aux ((s0, len) :: acc) rest
    | _ -> failwith "seeds_as_ranges"
  in
  aux [] seeds
;;

let invert_mappings mappings =
  let invert_triplet (des0, src0, len) = src0, des0, len in
  mappings
  |> List.map ~f:(fun (mapping_of, triplets) ->
       mapping_of, triplets |> List.map ~f:invert_triplet)
  |> List.rev
;;

let translate src mappings =
  let rec aux src = function
    | _, [] -> src
    | mapping_of, (des0, src0, len) :: triplets ->
      if src >= src0 && src - src0 < len
      then des0 + (src - src0)
      else aux src (mapping_of, triplets)
  in
  List.fold_left mappings ~init:src ~f:aux
;;

let part1 buf =
  let seeds, mappings = parse_all_input buf in
  let min_location =
    seeds
    |> List.map ~f:(fun seed -> translate seed mappings)
    |> List.fold_left ~init:Int.max_value_30_bits ~f:Int.min
  in
  Stdio.printf "%d\n" min_location
;;

let part2 buf =
  let seeds, mappings = parse_all_input buf in
  let seed_ranges = seeds_as_ranges seeds in
  let inv_mappings = invert_mappings mappings in
  let reachable loc =
    let seed = translate loc inv_mappings in
    seed_ranges
    |> List.exists ~f:(fun (seed0, len) -> seed >= seed0 && seed < seed0 + len)
  in
  (* search in the [lo, hi) range.
     pre-condition: reachable hi (assumed) *)
  let rec search lo hi =
    if lo = hi
    then lo
    else (
      let mid = lo + ((hi - lo) / 2) in
      if reachable mid
      then search lo mid
      else if reachable lo
      then lo
      else search (lo + 1) hi)
  in
  let min_location = search 0 Int.max_value_30_bits in
  Stdio.printf "%d\n" min_location
;;
