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
