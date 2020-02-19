#include <cassert>
#include <cstdio>
#include <iostream>  // for getline
#include <string>

#include "intcode.h"

struct Game {
  explicit Game(Program program)
      : _cpu(std::move(program)),
        _all_items({
            /* clang-format off */
            "coin",
            "fuel cell", 
            "cake", 
            "pointer", 
            "boulder", 
            "mutex", 
            "antenna", 
            "tambourine",
            /* clang-format on */
        }) {}

  std::string executeProgramToAssesSituation() {
    std::string output;
    _cpu.runConsumingOutput(output);
    return output;
  }

  std::string runCommand(const std::string &command) {
    _cpu.pushInput(command.c_str());
    _cpu.pushInput('\n');
    return executeProgramToAssesSituation();
  }

  std::string runMacro(const std::vector<std::string> &macro, bool verbose) {
    for (int i = 0; i + 1 < macro.size(); i++) {
      if (verbose) {
        printf("%s\n", macro[i].c_str());
      }
      const std::string output = runCommand(macro[i]);
      if (verbose) {
        printf("%s", output.c_str());
      }
    }
    // execute the last command and return its output
    if (!macro.empty()) {
      return runCommand(macro[macro.size() - 1]);
    }
    return "";
  }

  bool isCorrectWeight(uint32_t inventory, bool verbose) {
    std::vector<std::string> macro;
    // drop all items
    for (int i = 0; i < _all_items.size(); i++) {
      std::string command = "drop ";
      command += _all_items[i];
      macro.emplace_back(std::move(command));
    }
    // take all in inventory
    for (int i = 0; i < _all_items.size(); i++) {
      if (inventory & (0x1 << i)) {
        std::string command = "take ";
        command += _all_items[i];
        macro.emplace_back(std::move(command));
      }
    }
    // move east (to presure sensitive floor)
    macro.emplace_back("east");

    const std::string output = runMacro(macro, verbose);
    printf("%s", output.c_str());

    const char LIGHTER_ALERT[] =
        "Alert! Droids on this ship are lighter than the detected value!";
    const char HEAVIER_ALERT[] =
        "Alert! Droids on this ship are heavier than the detected value!";
    return !strstr(output.c_str(), LIGHTER_ALERT) &&
           !strstr(output.c_str(), HEAVIER_ALERT);
  }

  void run(bool verbose) {
    /* clang-format off */
    std::vector<std::string> take_everything = {
      // Hull Breach
      "west",
      "west",
      "west",
      "take coin",
      "east",
      "east",
      "east",
      // Hull Breach
      "north",
      "north",
      "take mutex",
      "east",
      "take antenna",
      "west",
      "south",
      // Corridor
      "east",
      "take cake",
      "east",
      "north",
      "take pointer",
      "south",
      "west",
      "west",
      "south",
      // Hull Breach
      "east",
      "east",
      "take tambourine",
      "east",
      "take fuel cell",
      "east",
      "take boulder",
      "north",
      "inv",
    };
    /* clang-format on */

    std::string output = executeProgramToAssesSituation();
    printf("%s", output.c_str());

    // take everything
    output = runMacro(take_everything, verbose);
    printf("%s", output.c_str());

    // try all combinations until it works
    uint32_t inventory = 0;
    for (; inventory < (0x1 << _all_items.size()); inventory++) {
      if (isCorrectWeight(inventory, verbose)) {
        return;
      }
    }

    std::string command;
    for (;;) {
      command.clear();
      getline(std::cin, command);
      output = runCommand(command);
      printf("%s", output.c_str());
    }
  }

 private:
  CPU _cpu;
  std::vector<std::string> _all_items;
};

int main() {
  auto input = fopen("25/in", "r");

  Program program;
  Word code;
  while (fscanf(input, "%lld", &code)) {
    program.push_back(code);
    char c = getc(input);
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  fclose(input);

  Game game(std::move(program));
  game.run(/* verbose */ true);

  return 0;
}
