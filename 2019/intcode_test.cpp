#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>

#include "intcode.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

TEST_CASE("Intcode utilities") {
  SECTION("Parsing a program") {
    REQUIRE(parseProgram("") == std::vector<long long>({}));
    REQUIRE(parseProgram("1") == std::vector<long long>({1}));
    REQUIRE(parseProgram("1,2") == std::vector<long long>({1, 2}));
    REQUIRE(parseProgram("1,2,3,4,5,6") ==
            std::vector<long long>({1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Day 02: 1202 Program Alarm", "[intcode]") {
  SECTION("Basic example program") {
    VM vm("1,9,10,3,2,3,11,0,99,30,40,50");
    vm.run();
    REQUIRE(vm.status == HALTED);
  }

  SECTION("Full program") {
    VM vm(
        "1,0,0,3,1,1,2,3,1,3,4,3,1,5,0,3,2,9,1,19,1,9,19,23,1,23,5,27,2,27,10,"
        "31,1,6,31,35,1,6,35,39,2,9,39,43,1,6,43,47,1,47,5,51,1,51,13,55,1,55,"
        "13,59,1,59,5,63,2,63,6,67,1,5,67,71,1,71,13,75,1,10,75,79,2,79,6,83,2,"
        "9,83,87,1,5,87,91,1,91,5,95,2,9,95,99,1,6,99,103,1,9,103,107,2,9,107,"
        "111,1,111,6,115,2,9,115,119,1,119,6,123,1,123,9,127,2,127,13,131,1,"
        "131,9,135,1,10,135,139,2,139,10,143,1,143,5,147,2,147,6,151,1,151,5,"
        "155,1,2,155,159,1,6,159,0,99,2,0,14,0");
    *vm.derefDest(1) = 12;
    *vm.derefDest(2) = 2;
    vm.run();
    REQUIRE(vm.status == HALTED);
    REQUIRE(vm.deref(0) == 6627023);
  }
}

TEST_CASE("Day 5: Sunny with a Chance of Asteroids", "[intcode]") {
  SECTION("Basic addressing mode examples") {
    {
      VM vm("1002,4,3,4,33");  // mem[4] = 3 * 33
      vm.run();
      REQUIRE(vm.status == HALTED);
    }
    {
      VM vm("1101,100,-1,4,0");  // mem[4] = 100 + -1
      vm.run();
      REQUIRE(vm.status == HALTED);
    }
  }

  SECTION("Position mode examples") {
    auto is_equal_to_8 = "3,9,8,9,10,9,4,9,99,-1,8";
    REQUIRE(runProgramAndGetFirstOutput(is_equal_to_8, 7) == 0);
    REQUIRE(runProgramAndGetFirstOutput(is_equal_to_8, 8) == 1);
    REQUIRE(runProgramAndGetFirstOutput(is_equal_to_8, 9) == 0);

    auto less_than_8 = "3,9,7,9,10,9,4,9,99,-1,8";
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 7) == 1);
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 8) == 0);
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 9) == 0);
  }

  SECTION("Immediate mode examples") {
    auto equal_to_8 = "3,3,1108,-1,8,3,4,3,99";
    REQUIRE(runProgramAndGetFirstOutput(equal_to_8, 7) == 0);
    REQUIRE(runProgramAndGetFirstOutput(equal_to_8, 8) == 1);
    REQUIRE(runProgramAndGetFirstOutput(equal_to_8, 9) == 0);

    auto less_than_8 = "3,3,1107,-1,8,3,4,3,99";
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 7) == 1);
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 8) == 0);
    REQUIRE(runProgramAndGetFirstOutput(less_than_8, 9) == 0);
  }

  SECTION("Jump example with position mode") {
    auto is_non_zero = "3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9";
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 0) == 0);
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 1) == 1);
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 666) == 1);
  }

  SECTION("Jump example with immediate mode") {
    auto is_non_zero = "3,3,1105,-1,9,1101,0,0,12,4,12,99,1";
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 0) == 0);
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 1) == 1);
    REQUIRE(runProgramAndGetFirstOutput(is_non_zero, 666) == 1);
  }

  SECTION("Larger example") {
    auto cmp_8_plus_1000 =
        "3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,"
        "1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,"
        "1105,1,46,98,99";
    REQUIRE(runProgramAndGetFirstOutput(cmp_8_plus_1000, 7) == 999);
    REQUIRE(runProgramAndGetFirstOutput(cmp_8_plus_1000, 8) == 1000);
    REQUIRE(runProgramAndGetFirstOutput(cmp_8_plus_1000, 9) == 1001);
  }

  SECTION("Full program") {
    auto diagnostic_program =
        "3,225,1,225,6,6,1100,1,238,225,104,0,1102,7,85,225,1102,67,12,225,102,"
        "36,65,224,1001,224,-3096,224,4,224,1002,223,8,223,101,4,224,224,1,224,"
        "223,223,1001,17,31,224,1001,224,-98,224,4,224,1002,223,8,223,101,5,"
        "224,224,1,223,224,223,1101,86,19,225,1101,5,27,225,1102,18,37,225,2,"
        "125,74,224,1001,224,-1406,224,4,224,102,8,223,223,101,2,224,224,1,224,"
        "223,223,1102,13,47,225,1,99,14,224,1001,224,-98,224,4,224,102,8,223,"
        "223,1001,224,2,224,1,224,223,223,1101,38,88,225,1102,91,36,224,101,-"
        "3276,224,224,4,224,1002,223,8,223,101,3,224,224,1,224,223,223,1101,59,"
        "76,224,1001,224,-135,224,4,224,102,8,223,223,1001,224,6,224,1,223,224,"
        "223,101,90,195,224,1001,224,-112,224,4,224,102,8,223,223,1001,224,7,"
        "224,1,224,223,223,1102,22,28,225,1002,69,47,224,1001,224,-235,224,4,"
        "224,1002,223,8,223,101,5,224,224,1,223,224,223,4,223,99,0,0,0,677,0,0,"
        "0,0,0,0,0,0,0,0,0,1105,0,99999,1105,227,247,1105,1,99999,1005,227,"
        "99999,1005,0,256,1105,1,99999,1106,227,99999,1106,0,265,1105,1,99999,"
        "1006,0,99999,1006,227,274,1105,1,99999,1105,1,280,1105,1,99999,1,225,"
        "225,225,1101,294,0,0,105,1,0,1105,1,99999,1106,0,300,1105,1,99999,1,"
        "225,225,225,1101,314,0,0,106,0,0,1105,1,99999,107,226,226,224,102,2,"
        "223,223,1006,224,329,1001,223,1,223,1107,677,226,224,1002,223,2,223,"
        "1005,224,344,101,1,223,223,108,677,226,224,102,2,223,223,1006,224,359,"
        "101,1,223,223,7,677,226,224,102,2,223,223,1005,224,374,101,1,223,223,"
        "1008,677,226,224,1002,223,2,223,1006,224,389,1001,223,1,223,7,226,677,"
        "224,102,2,223,223,1005,224,404,101,1,223,223,1007,226,226,224,102,2,"
        "223,223,1006,224,419,101,1,223,223,7,226,226,224,102,2,223,223,1005,"
        "224,434,1001,223,1,223,8,226,226,224,1002,223,2,223,1006,224,449,101,"
        "1,223,223,1007,677,677,224,102,2,223,223,1006,224,464,101,1,223,223,"
        "1007,226,677,224,1002,223,2,223,1006,224,479,101,1,223,223,108,226,"
        "226,224,102,2,223,223,1005,224,494,1001,223,1,223,1108,677,677,224,"
        "102,2,223,223,1005,224,509,1001,223,1,223,107,226,677,224,1002,223,2,"
        "223,1005,224,524,101,1,223,223,1108,677,226,224,1002,223,2,223,1005,"
        "224,539,1001,223,1,223,1008,677,677,224,1002,223,2,223,1006,224,554,"
        "101,1,223,223,1008,226,226,224,102,2,223,223,1005,224,569,1001,223,1,"
        "223,8,677,226,224,102,2,223,223,1006,224,584,101,1,223,223,107,677,"
        "677,224,102,2,223,223,1006,224,599,101,1,223,223,8,226,677,224,102,2,"
        "223,223,1006,224,614,101,1,223,223,1107,226,677,224,102,2,223,223,"
        "1006,224,629,101,1,223,223,108,677,677,224,1002,223,2,223,1005,224,"
        "644,1001,223,1,223,1107,226,226,224,102,2,223,223,1005,224,659,101,1,"
        "223,223,1108,226,677,224,102,2,223,223,1005,224,674,101,1,223,223,4,"
        "223,99,226";
    REQUIRE(runProgramAndGetOutput(diagnostic_program, 1) ==
            std::vector<long long>({0, 0, 0, 0, 0, 0, 0, 0, 0, 9775037}));
    REQUIRE(runProgramAndGetFirstOutput(diagnostic_program, 5) == 15586959);
  }
}

long long runAllAmplifiers(std::vector<long long> program,
                           std::vector<int> phases) {
  std::vector<VM> vms;
  vms.emplace_back(std::move(program));
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);

  for (int i = 0; i < 5; i++) {
    vms[i].pushInput(phases[i]);
  }
  vms[0].pushInput(0);

  long long out = 0;
  for (int i = 0; i < 5; i++) {
    VM &vm = vms[i];
    vm.run();
    out = vm.consumeOutput();
    assert(vm.status == HALTED);
    if (i < 4) {
      vms[i + 1].pushInput(out);
    }
  }

  return out;
}

long long runAllAmplifiersInLoop(std::vector<long long> program,
                                 std::vector<int> phases) {
  std::vector<VM> vms;
  vms.emplace_back(std::move(program));
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  auto *e = &vms[4];

  for (int i = 0; i < 5; i++) {
    vms[i].pushInput(phases[i]);
  }
  vms[0].pushInput(0);

  long long e_out = 0;
  long long out = 0;
  int i = 0;
  for (;;) {
    VM &vm = vms[i];
    vm.runUntilOutput();
    if (vm.status == HALTED) {
      break;
    }
    assert(vm.hasOutput());
    out = vm.consumeOutput();
    if (&vm == e) {
      e_out = out;
    }
    int next = (i + 1) % 5;
    vms[next].pushInput(out);
    i = next;
  }

  return e_out;
}

TEST_CASE("Day 7: Amplification Circuit", "[intcode]") {
  SECTION("Basic examples (without feedback loop)") {
    auto prog = parseProgram("3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0");
    REQUIRE(runAllAmplifiers(std::move(prog), {4, 3, 2, 1, 0}) == 43210);

    prog = parseProgram(
        "3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5,23,23,1,24,23,23,4,23,99,"
        "0,0");
    REQUIRE(runAllAmplifiers(std::move(prog), {0, 1, 2, 3, 4}) == 54321);

    prog = parseProgram(
        "3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,1002,33,7,33,1,33,"
        "31,31,1,32,31,31,4,31,99,0,0,0");
    REQUIRE(runAllAmplifiers(prog, {1, 0, 4, 3, 2}) == 65210);
  }

  SECTION("Basic examples (with feedback loop)") {
    auto prog = parseProgram(
        "3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,27,4,27,1001,28,-1,28,"
        "1005,28,6,99,0,0,5");
    REQUIRE(runAllAmplifiersInLoop(std::move(prog), {9, 8, 7, 6, 5}) ==
            139629729);

    prog = parseProgram(
        "3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,-5,"
        "54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,53,"
        "1001,56,-1,56,1005,56,6,99,0,0,0,0,10");
    REQUIRE(runAllAmplifiersInLoop(std::move(prog), {9, 7, 8, 5, 6}) == 18216);
  }

  SECTION("Full program") {
    auto prog = parseProgram(
        "3,8,1001,8,10,8,105,1,0,0,21,46,59,72,93,110,191,272,353,434,99999,3,"
        "9,101,4,9,9,1002,9,3,9,1001,9,5,9,102,2,9,9,1001,9,5,9,4,9,99,3,9,"
        "1002,9,5,9,1001,9,5,9,4,9,99,3,9,101,4,9,9,1002,9,4,9,4,9,99,3,9,102,"
        "3,9,9,101,3,9,9,1002,9,2,9,1001,9,5,9,4,9,99,3,9,1001,9,2,9,102,4,9,9,"
        "101,2,9,9,4,9,99,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,102,2,9,9,"
        "4,9,3,9,102,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,"
        "9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,101,2,9,9,4,9,99,3,9,"
        "101,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,"
        "9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,"
        "9,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,99,3,9,101,2,9,9,4,9,3,9,1001,9,"
        "1,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,"
        "102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,"
        "9,3,9,101,2,9,9,4,9,99,3,9,102,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,1001,"
        "9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,"
        "9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,2,9,4,"
        "9,99,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,2,9,4,9,3,9,102,"
        "2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,"
        "1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,99");
    REQUIRE(runAllAmplifiers(prog, {0, 2, 4, 3, 1}) == 21000);
    REQUIRE(runAllAmplifiersInLoop(prog, {6, 7, 9, 8, 5}) == 61379886);
  }
}

int main(int argc, char *argv[]) {
  int catch_status = Catch::Session().run(argc, argv);
  if (catch_status) {
    return catch_status;
  }

  // add quick experiments here

  return 0;
}
