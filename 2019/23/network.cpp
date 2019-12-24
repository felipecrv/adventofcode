#include <cassert>
#include <cstdio>

#include "intcode.h"

struct NAT {
  void recv(long long x, long long y) {
    if (!received) {
      printf("NAT: first Y received by NAT: %lld\n", y);
    }
    received = true;
    received_x = x;
    received_y = y;
  }

  void sendLastPacketTo(CPU &dest) {
    assert(hasReceivedPacket());
    printf("NAT: send[%lld] %lld %lld\n", 0ll, received_x, received_y);
    dest.pushInput(received_x);
    dest.pushInput(received_y);

    if (sent && received_y == sent_y) {
      printf("NAT: sending the same Y in a row: %lld\n", received_y);
      exit(0);
    }
    sent = true;
    sent_y = received_y;
  }

  bool hasReceivedPacket() const { return received; }

 private:
  bool received = false;
  long long received_x;
  long long received_y;

  bool sent = false;
  long long sent_y;
};

struct Network {
  explicit Network(Program program) : _program(std::move(program)) {
    for (auto &cpu : _cpu) {
      cpu.clearState();
      cpu.loadProgram(_program);
    }
  }

  void assignAddresses() {
    // give addresses to all CPUs
    for (int i = 0; i < 50; i++) {
      CPU &cpu = _cpu[i];
      cpu.run();
      assert(cpu.status == PENDING_IN);
      assert(cpu.op == IN);

      // give it the address
      cpu.pushInput(i);

      // ensure it consumes the address
      cpu.tick();

      assert(!cpu.hasOutput());
      assert(!cpu.hasInput());
    }
  }

  // return true if network is idle
  bool tick() {
    bool sent_packet = false;
    int stuck_count = 0;

    for (int i = 0; i < 50; i++) {
      CPU &cpu = _cpu[i];
      assert(cpu.status != HALTED);

      // If there's a buffered package and the CPU is in PENDING_IN,
      // ensure the package is consume before continuing.
      if (cpu.status == PENDING_IN && cpu.hasInput()) {
        bool consumed = cpu.consumePacket();
        assert(consumed);
      }

      cpu.runUntilIO();  // run until IN or OUT

      // if found an unfulfilled IN, give it a -1
      if (cpu.status == PENDING_IN) {
        cpu.pushInput(-1);
        stuck_count += 1;
        continue;
      }

      // if found an OUT, send that as a package
      if (cpu.hasOutput()) {
        cpu.runUntilOutput();  // x
        cpu.runUntilOutput();  // y
        long long addr = cpu.consumeOutput();
        long long x = cpu.consumeOutput();
        long long y = cpu.consumeOutput();

        printf("%d: send[%lld] %lld %lld\n", i, addr, x, y);
        if (addr < 50) {
          CPU &dest = _cpu[addr];
          assert(dest.status != HALTED);
          dest.pushInput(x);
          dest.pushInput(y);
        }
        if (addr == 255) {
          _nat.recv(x, y);
        }
        sent_packet = true;
        continue;
      }
    }

    return !sent_packet && stuck_count == 50;
  }

  void run() {
    assignAddresses();

    for (;;) {
      bool network_idle = tick();

      if (network_idle) {
        if (_nat.hasReceivedPacket()) {
          _nat.sendLastPacketTo(_cpu[0]);
        }
      }
    }
  }

 private:
  NAT _nat;
  CPU _cpu[50];
  Program _program;
};

int main() {
  Program program;
  Word code;
  while (scanf("%lld", &code)) {
    program.push_back(code);
    int c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  Network net(std::move(program));
  net.run();

  return 0;
}
