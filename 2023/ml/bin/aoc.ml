open Runner

let _ =
  register ~day:1 (module StringInput (Day01));
  register ~day:2 (module SedlexInput (Day02));
  main ()
;;
