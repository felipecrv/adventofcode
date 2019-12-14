#include <algorithm>
#include <cassert>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#define TRILLION 1000000000000

template <typename MapType>
const typename MapType::mapped_type *lookup(
    const MapType &m, const typename MapType::key_type &k) {
  auto it = m.find(k);
  if (it != m.end()) {
    return &it->second;
  }
  return nullptr;
}

using Stock = long long;
using string = std::string;
template <typename K, typename V>
using Map = std::unordered_map<K, V>;
template <typename T>
using vector = std::vector<T>;

struct Reaction {
  Map<string, Stock> ingredients;
  string result;  // chemical produced by this Reaction
  Stock yield;    // amount of result produced per Reaction

  void clear() {
    ingredients.clear();
    yield = 0;
    result.clear();
  }
};

struct Nanofactory {
  explicit Nanofactory(Map<string, Reaction> reaction_for)
      : _reaction_for(std::move(reaction_for)) {
    for (auto &[result, _] : _reaction_for) {
      _sorted_chemicals.push_back(result);
    }

    std::sort(_sorted_chemicals.begin(), _sorted_chemicals.end(),
              [this](const string &a, const string &b) {
                return distanceToOre(a) < distanceToOre(b);
              });

    for (auto &chemical : _sorted_chemicals) {
      _ingredients_for[chemical] = gatherIngredientsFor(chemical);
    }
  }

  Stock oreStock() { return _stock_of["ORE"]; }
  void setOreStock(Stock stock) { _stock_of["ORE"] = stock; }
  void restart() { _stock_of.clear(); }

  void make(Stock to_make, const string &chemical) {
    // printf("make(%lld, %s)\n", to_make, chemical.c_str());
    auto &stock = _stock_of[chemical];

    if (chemical == "ORE") {
      stock -= to_make;
      return;
    }

    if (stock > 0) {
      auto take_from_stock = std::min(stock, to_make);
      to_make -= take_from_stock;
      stock -= take_from_stock;
    }
    if (to_make == 0) {
      return;
    }

    auto *r = lookup(_reaction_for, chemical);
    const Stock repeat = (to_make - 1) / r->yield + 1;  // ceil(to_make / yield)
    const auto &ingredients = *lookup(_ingredients_for, chemical);
    for (auto &i : ingredients) {
      make(repeat * *lookup(r->ingredients, i), i);
    }
    stock += repeat * r->yield - to_make;
  }

 private:
  Map<string, Reaction> _reaction_for;
  vector<string> _sorted_chemicals;
  Map<string, vector<string>> _ingredients_for;

  Map<string, Stock> _stock_of;

  vector<string> gatherIngredientsFor(const string &chemical) {
    if (chemical == "ORE") {
      return vector<string>({"ORE"});
    }

    vector<string> ingredients;
    auto *r = lookup(_reaction_for, chemical);
    for (const auto &[chemical, _] : r->ingredients) {
      ingredients.push_back(chemical);
    }
    std::sort(ingredients.begin(), ingredients.end(),
              [this](const string &a, const string &b) {
                return distanceToOre(a) < distanceToOre(b);
              });
    return ingredients;
  }

  int distanceToOre(const string &chemical) const {
    if (chemical == "ORE") {
      return 0;
    }
    int dist = 0x3f3f3f3f;
    auto *r = lookup(_reaction_for, chemical);
    for (auto &[chemical, _] : r->ingredients) {
      dist = std::min(dist, distanceToOre(chemical));
    }
    return dist + 1;
  }
};

bool canProduceWithATrillionOre(Nanofactory &factory, Stock can_produce) {
  printf("Try to make %lld FUEL with a trillion ORE... ", can_produce);
  factory.restart();
  factory.setOreStock(TRILLION);
  factory.make(can_produce, "FUEL");
  const bool can = factory.oreStock() >= 0;
  printf(factory.oreStock() >= 0 ? "OK\n" : "NOT OK\n");
  return can;
}

int main() {
  Map<string, Reaction> reaction_for;
  for (;;) {
    Reaction r;
    for (;;) {
      int quantity;
      char name[50];
      if (scanf("%d %s", &quantity, name) == 2) {
        int name_len = strlen(name);
        if (name[name_len - 1] == ',') {
          name[name_len - 1] = 0;
          r.ingredients[name] = quantity;
        } else {
          r.ingredients[name] = quantity;
          scanf(" => %d %s", &quantity, name);
          r.result = name;
          r.yield = quantity;
          reaction_for[name] = std::move(r);
          break;
        }
      } else {
        goto end;
      }
    }
    r.clear();
    continue;
  end:
    break;
  }

  Nanofactory factory(std::move(reaction_for));

  factory.make(1, "FUEL");
  const Stock needed_ore_for_one_fuel = -factory.oreStock();
  printf("%lld ORE necessary for 1 FUEL\n", needed_ore_for_one_fuel);

  Stock lo = TRILLION / needed_ore_for_one_fuel;
  printf("Lower bound of FUEL from a trillion ORE %lld\n", lo);
  Stock hi = lo * 2;
  while (canProduceWithATrillionOre(factory, hi)) {
    hi *= 2;
  }
  printf("Upper bound of FUEL from a trillion ORE %lld\n", hi);

  // invariant: can(lo)
  while (lo < hi) {
    Stock m = (hi - lo - 1) / 2 + lo + 1;
    if (canProduceWithATrillionOre(factory, m)) {
      lo = m;
    } else {
      hi = m - 1;
    }
  }
  printf("Maximum FUEL produced for a trillion ORE %lld\n", lo);

  return 0;
}
