#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
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

  long long num() {
    ws();
    long long v = 0;

    while (p < s.size() && std::isdigit(static_cast<unsigned char>(s[p]))) {
      v = v * 10 + (s[p++] - '0');
    }

    return v;
  }

  std::string_view str() {
    ws();
    ++p;
    size_t b = p;

    while (s[p] != '"') {
      ++p;
    }

    return s.substr(b, p++ - b);
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

struct Op {
  char code;
  int a, b, c;
};

// TODO: run ops, return the event log ("d<id>", "u<n>", "leak<id>").
// Use shared_ptr/weak_ptr; free fields without recursion.
static std::vector<std::string> solve(int nvars, const std::vector<Op>& ops) {
  (void)nvars;
  (void)ops;

  return {};
}

int main() {
  std::ios::sync_with_stdio(false);
  std::string line(std::istreambuf_iterator<char>(std::cin), {});
  Cursor c = at(line, "vars");
  int nvars = (int)c.num();
  std::vector<Op> ops;
  c = at(line, "ops");
  c.arr([&] {
    Op o{};
    int i = 0;
    c.arr([&] {
      if (i == 0) {
        o.code = c.str()[0];
      } else {
        (i == 1 ? o.a : i == 2 ? o.b : o.c) = (int)c.num();
      }

      ++i;
    });
    ops.push_back(o);
  });
  auto res = solve(nvars, ops);
  std::string out = "[";

  for (size_t i = 0; i < res.size(); ++i) {
    out += (i ? ",\"" : "\"") + res[i] + "\"";
  }

  std::cout << out << "]";

  return 0;
}
