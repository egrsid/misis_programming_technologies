#include <cctype>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

struct Cursor {
  std::string_view s;
  size_t p;

  void ws() {
    while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p]))) {
      ++p;
    }
  }

  bool eat(char c) {
    ws();

    if (p < s.size() && s[p] == c) {
      ++p;

      return true;
    }

    return false;
  }

  std::string str() {
    ws();
    ++p;
    size_t b = p;

    while (s[p] != '"') {
      ++p;
    }

    return std::string(s.substr(b, p++ - b));
  }

  template <class F>
  void arr(F f) {
    eat('[');

    if (eat(']')) {
      return;
    }

    do {
      f();
    } while (eat(','));
    eat(']');
  }
};

static Cursor at(std::string_view s, std::string_view key) {
  const std::string k = "\"" + std::string(key) + "\"";

  for (size_t pos = s.find(k); pos != std::string::npos;
       pos = s.find(k, pos + 1)) {
    Cursor c{s, pos + k.size()};

    if (c.eat(':')) {
      return c;
    }
  }

  return Cursor{s, s.size()};
}

using Op = std::vector<std::string>;

using Names = std::unordered_set<std::string>;

// TODO: run ops, return the event log ending with "result ...".
// Use an RAII scope, exceptions, a concept and std::variant.
static std::vector<std::string> solve(Names fo, Names fc, Names fw,
                                      const std::vector<Op>& ops) {
  (void)fo;
  (void)fc;
  (void)fw;
  (void)ops;

  return {};
}

int main() {
  std::ios::sync_with_stdio(false);
  std::string line(std::istreambuf_iterator<char>(std::cin), {});
  Names sets[3];
  const char* keys[3] = {"fail_open", "fail_close", "fail_work"};

  for (int i = 0; i < 3; ++i) {
    Cursor c = at(line, keys[i]);
    c.arr([&] { sets[i].insert(c.str()); });
  }

  std::vector<Op> ops;
  Cursor c = at(line, "ops");
  c.arr([&] {
    Op o;
    c.arr([&] { o.push_back(c.str()); });
    ops.push_back(std::move(o));
  });
  auto res =
      solve(std::move(sets[0]), std::move(sets[1]), std::move(sets[2]), ops);
  std::string out = "[";

  for (size_t i = 0; i < res.size(); ++i) {
    out += (i ? ",\"" : "\"") + res[i] + "\"";
  }

  std::cout << out << "]";

  return 0;
}
