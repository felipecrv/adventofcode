open Runner

let _ =
  register ~day:1 (module StringInput (Day01));
  register ~day:2 (module SedlexInput (Day02));
  register ~day:3 (module SedlexInput (Day03));
  register ~day:4 (module SedlexInput (Day04));
  register ~day:5 (module SedlexInput (Day05));
  register ~day:6 (module SedlexInput (Day06));
  register ~day:7 (module SedlexInput (Day07));
  (* Day 08 was solved in C++ *)
  register ~day:9 (module SedlexInput (Day09));
  (* Day 10 was solved in C++ *)
  (* Day 11 was solved in C++ *)
  register ~day:12 (module SedlexInput (Day12));
  (* Day 13 was solved in C++ *)
  (* Day 14 was solved in C++ *)
  register ~day:15 (module StringInput (Day15));
  main ()
;;
