open Base

let fail buf msg =
  let pos, _ = Sedlexing.lexing_positions buf in
  let line, col = pos.pos_lnum, pos.pos_cnum - pos.pos_bol in
  let next =
    let sample = [%sedlex.regexp? Rep (any, 1 .. 100)] in
    match%sedlex buf with
    | sample -> Sedlexing.Latin1.lexeme buf
    | _ -> ""
  in
  let open Stdio.Out_channel in
  fprintf stderr "%d:%d: %s:\n\027[91m%s\027[0m...\n" line col msg next;
  Stdlib.exit 1
;;

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
  | _ -> fail buf "token"
;;

type triplet = int * int * int (* des0, src0, len *)

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
    | Newline | End ->
      let compare (_, src0, _) (_, src0', _) =
        let cmp = Int.compare src0 src0' in
        assert (cmp <> 0);
        cmp
      in
      List.sort acc ~compare
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
  | _ -> fail buf "parse_all_input"
;;

let print_mappings mappings =
  let print_triplet (des0, src0, len) =
    Stdio.printf
      "  [%3d, %3d) -> [%3d, %3d)  |%2d|\n"
      src0
      (src0 + len)
      des0
      (des0 + len)
      len
  in
  List.iter mappings ~f:(fun (mapping_of, triplets) ->
    Stdio.printf "%s map:\n" mapping_of;
    List.iter triplets ~f:print_triplet)
;;

let intersection a b =
  match a, b with
  | (a0, alen), (b0, blen) ->
    let c0 = Int.max a0 b0 in
    let c1 = Int.min (a0 + alen) (b0 + blen) in
    if c0 < c1 then Some (c0, c1 - c0) else None
;;

let split_query (a0, alen) b0 =
  let a1 = a0 + alen in
  let before =
    if a0 < b0
    then (
      let c1 = Int.min a1 b0 in
      Some (a0, c1 - a0))
    else None
  in
  let after =
    if a0 < b0
    then (
      let c0 = b0 in
      let c1 = a1 in
      c0, c1 - c0 (* could be negative length *))
    else a0, alen
  in
  before, after
;;

let min_opt x x_opt =
  match x_opt with
  | None -> Some x
  | Some x' -> Some (Int.min x x')
;;

let is_empty = function
  | _, len -> len <= 0
;;

let rec min_location acc query mappings =
  let rec aux0 acc0 query0 triplets0 mappings1 =
    if is_empty query0
    then acc0
    else (
      match triplets0 with
      | [] -> acc0
      | (des0, src0, len) :: triplets0' ->
        (match split_query query0 src0 with
         | Some before0, after0 ->
           let acc0' =
             let src', _ = before0 in
             let min1 = min_opt src' acc0 in
             let query1 = before0 in
             min_location min1 query1 mappings1
           in
           aux0 acc0' after0 triplets0 mappings1
         | None, after0 ->
           assert (fst after0 >= src0);
           (match intersection after0 (src0, len) with
            | None -> aux0 acc0 after0 triplets0' mappings1
            | Some (src', len') ->
              let src'' = des0 + (src' - src0) in
              let len'' = len - len' in
              let acc0' =
                let min1 = min_opt src'' acc0 in
                let query1 = src'', len'' in
                min_location min1 query1 mappings1
              in
              aux0 acc0' after0 triplets0' mappings1)))
  in
  match mappings with
  | [] -> acc
  | triplets0 :: mappings1 -> aux0 None query triplets0 mappings1
;;

let translate src mappings =
  let rec aux src = function
    | _, [] -> src
    | mapping_of, (des0, src0, len) :: triplets ->
      if src >= src0 && src - src0 < len
      then (
        let res = des0 + (src - src0) in
        (* Stdio.printf "translate %s %d -> %d\n" mapping_of src res; *)
        res)
      else aux src (mapping_of, triplets)
  in
  List.fold_left mappings ~init:src ~f:aux
;;

let invert_mappings mappings =
  let invert_triplet (des0, src0, len) = src0, des0, len in
  mappings
  |> List.map ~f:(fun (mapping_of, triplets) ->
       mapping_of, List.map ~f:invert_triplet triplets)
  |> List.rev
;;

let contains_seed seed =
  List.exists ~f:(fun (seed0, len) -> seed >= seed0 && seed < seed0 + len)
;;

let solve seed_ranges inv_mappings =
  let reachable loc =
    let seed = translate loc inv_mappings in
    contains_seed seed seed_ranges
  in
  (* pre: reachable limit
     post: last <= limit *)
  let discover start limit =
    Stdio.printf "discover (%d, %d)... " start limit; let rec aux inc =
      let last = start + inc - 1 in
      if last >= limit then limit else if reachable last then last else aux (inc * 2)
    in
    aux 1
  in
  (* [lo, hi) /\ reachable hi *)
  let rec binary_search lo hi =
    if lo = hi
    then lo
    else (
      let mid = lo + ((hi - lo) / 2) in
      if reachable mid
      then binary_search lo mid
      else if reachable lo
      then lo
      else binary_search (lo + 1) hi)
  in
  let hi = discover 0 (Int.max_value_30_bits / 2) in
  Stdio.printf "\ndiscovered %d\n" hi;
  binary_search 0 hi
;;

let part1 buf =
  let seeds, mappings = parse_all_input buf in
  Stdio.printf "seeds: %s\n" (String.concat ~sep:" " (List.map seeds ~f:Int.to_string));
  Stdio.Out_channel.flush Stdio.stdout;
  print_mappings mappings;
  let locations =
    List.map seeds ~f:(fun seed ->
      let res = translate seed mappings in
      Stdio.printf "\n";
      res)
  in
  Stdio.printf
    "locations: %s\n"
    (String.concat ~sep:" " (List.map locations ~f:Int.to_string));
  let min_location = List.fold_left locations ~init:(List.hd_exn locations) ~f:Int.min in
  Stdio.printf "min location: %d\n" min_location
;;

let seeds_as_ranges seeds =
  let rec aux acc = function
    | [] -> List.rev acc
    | s0 :: len :: rest -> aux ((s0, len) :: acc) rest
    | _ -> failwith "seeds_as_ranges"
  in
  aux [] seeds
;;

let lens_sum = List.fold_left ~init:0 ~f:(fun acc (_, len) -> acc + len)

let part2 buf =
  let seeds, mappings = parse_all_input buf in
  let ranges = seeds_as_ranges seeds in
  Stdio.printf "%d seeds\n" (lens_sum ranges);
  let min_location = solve ranges (invert_mappings mappings) in
  (* let min_location = min_location None (List.hd_exn ranges) mappings' in *)
  Stdio.printf "min_location: %d\n" min_location
;;
