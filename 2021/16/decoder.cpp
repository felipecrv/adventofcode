#include <cassert>
#include <cstdio>
#include <vector>

int hexval(char h) {
  if (h >= '0' && h <= '9') return h - '0';
  if (h >= 'A' && h <= 'F') return 10 + (h - 'A');
  assert(false && "invalid hexadecimal char");
  return -1;
}

const char *operatorName(int type_id) {
  switch (type_id) {
    case 0:
      return "+";
    case 1:
      return "*";
    case 2:
      return "min";
    case 3:
      return "max";
    case 5:
      return ">";
    case 6:
      return "<";
    case 7:
      return "=";
    case 8:  // identity (invented by me)
      return "id";
  }
  assert(false);
  return "";
}

struct Operation {
  explicit Operation(int type_id) : operator_(type_id) {
    assert(operator_ != 4);
  }

  void pushOperand(long long value) { ops.push_back(value); }

  long long eval() {
    switch (operator_) {
      case 0:  // +
        return reduce([](long long a, long long b) { return a + b; });
      case 1:  // *
        return reduce([](long long a, long long b) { return a * b; });
      case 2:  // min
        return reduce([](long long a, long long b) { return std::min(a, b); });
      case 3:  // max
        return reduce([](long long a, long long b) { return std::max(a, b); });
      case 5:  // >
        assert(ops.size() == 2);
        return ops[0] > ops[1] ? 1 : 0;
      case 6:  // <
        assert(ops.size() == 2);
        return ops[0] < ops[1] ? 1 : 0;
      case 7:  // =
        assert(ops.size() == 2);
        return ops[0] == ops[1] ? 1 : 0;
      case 8:
        assert(ops.size() == 1);
        return ops[0];
    }
    assert(false);
    return 0;
  }

  template <class Op>
  long long reduce(Op &&op) {
    assert(ops.size() >= 1);
    long long ret = ops[0];
    for (int i = 1; i < ops.size(); i++) {
      ret = op(ret, ops[i]);
    }
    return ret;
  }

  int operator_;
  std::vector<long long> ops;  // operands
};

using Packet = std::vector<int>;

#define EXPECT(expr) \
  if (!(expr)) {     \
    return false;    \
  }

class Parser {
 public:
  Parser(Packet &p) : p(p) {}

  bool peek(long long &out) const {
    if (i >= p.size() * 4) return false;
    const int h = p[i / 4];
    out <<= 1;
    out |= (h >> (3 - (i % 4))) & 0x1;
    return true;
  }

  bool next(long long &out) {
    const bool ret = peek(out);
    i += 1;
    return ret;
  }

  bool flag(long long &f) { return next(f) && next(f) && next(f); }

  bool halfByte(long long &half) {
    return next(half) && next(half) && next(half) && next(half);
  }

  bool literal(long long &lit) {
    for (;;) {
      long long bit = 0;
      EXPECT(next(bit));
      EXPECT(halfByte(lit));
      if (bit == 0) {
        break;
      }
    }
    return true;
  }

  bool number(int b, long long &out) {
    for (int j = 0; j < b; j++) {
      EXPECT(next(out));
    }
    return true;
  }

  bool packet(Operation &op) {
    long long version = 0;
    EXPECT(flag(version));
    version_sum += version;

    long long type_id = 0;
    EXPECT(flag(type_id));
    switch (type_id) {
      case 4: {  // literal
        long long lit = 0;
        EXPECT(literal(lit));
        printf("%lld ", lit);
        op.pushOperand(lit);
        break;
      }
      default: {
        Operation inner_op(type_id);
        putchar('(');
        long long bit = 0;
        EXPECT(next(bit));
        if (bit == 0) {
          // 15 bits -- length
          long long bitlen = 0;
          EXPECT(number(15, bitlen));
          const long long limit = i + bitlen;
          while (i < limit) {
            EXPECT(packet(inner_op));
          }
        } else {
          // 11 bits -- num sub-packets
          long long num_packets = 0;
          EXPECT(number(11, num_packets));
          for (int j = 0; j < num_packets; j++) {
            EXPECT(packet(inner_op));
          }
        }
        op.pushOperand(inner_op.eval());
        printf("%s)", operatorName(type_id));
        break;
      }
    }
    return true;
  }

  long long parse() {
    Operation op(/*identity*/ 8);
    if (!packet(op)) {
      assert(false);
      return 0;
    }
    return op.eval();
  }

  int i = 0;
  Packet &p;

  int version_sum = 0;
};

int main() {
  Packet packet;
  for (;;) {
    const char c = getchar();
    if (c == '\n' || c == EOF) {
      if (!packet.empty()) {
        Parser parser(packet);
        long long value = parser.parse();
        printf("\nversion sum %d\n", parser.version_sum);
        printf("eval %lld\n", value);
        packet.clear();
        putchar('\n');
      }
      if (c == '\n') {
        continue;
      }
      if (c == EOF) {
        break;
      }
    }
    packet.push_back(hexval(c));
  }
  return 0;
}
