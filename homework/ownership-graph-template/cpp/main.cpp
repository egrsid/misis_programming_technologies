#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <variant>

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
  int a;
  int b;
  int c;
};

struct Obj;

struct Slot {
  std::shared_ptr<Obj> strong;
  std::weak_ptr<Obj> weak;
};

std::vector<std::string> logs;
std::vector<std::shared_ptr<Obj>> toRelease;
bool releasing = false;

struct Obj {
  int id;
  std::vector<Slot> parts;

  ~Obj() {
    logs.push_back("d" + std::to_string(id));

    for (int i = parts.size() - 1; i >= 0; i--) {
      if (parts[i].strong != nullptr) {
        toRelease.push_back(parts[i].strong);
        parts[i].strong = nullptr;
      }
    }

    if (releasing) { return; }

    releasing = true;
    while (!toRelease.empty()) {
      std::shared_ptr<Obj> p = toRelease.back();
      toRelease.pop_back();
      p = nullptr;
    }

    releasing = false;
  }
};

std::shared_ptr<Obj> getObj(Slot& s) {
  if (s.strong != nullptr) { return s.strong; }
  return s.weak.lock();
}

int useAmount(Slot& s) {
  if (s.strong != nullptr) { return s.strong.use_count(); }
  return s.weak.use_count();
}

void setSlot(Slot& dst, Slot value) {
  Slot old = dst;
  dst = value;
}

// TODO: run ops, return the event log ("d<id>", "u<n>", "leak<id>").
// Use shared_ptr/weak_ptr; free fields without recursion.
static std::vector<std::string> solve(int nvars, const std::vector<Op>& ops) {
  std::vector<Slot> vars(nvars);
  std::vector<std::weak_ptr<Obj>> allObj;
  allObj.reserve(ops.size());

  for (int k = 0; k < ops.size(); k++) {
    char code = ops[k].code;
    int a = ops[k].a;
    int b = ops[k].b;
    int c = ops[k].c;

    if (code == 'N') {
      std::shared_ptr<Obj> obj = std::make_shared<Obj>();
      obj->id = allObj.size();
      obj->parts.resize(b);
      allObj.push_back(obj);

      Slot value;
      value.strong = obj;
      setSlot(vars[a], value);
    } else if (code == 'C') {
      setSlot(vars[a], vars[b]);
    } else if (code == 'M') {
      if (a != b) {
        Slot value = vars[b];
        vars[b] = Slot();
        setSlot(vars[a], value);
      }
    } else if (code == 'W') {
      Slot value;
      value.weak = getObj(vars[b]);
      setSlot(vars[a], value);
    } else if (code == 'L') {
      Slot value;
      value.strong = getObj(vars[b]);
      setSlot(vars[a], value);
    } else if (code == 'R') {
      setSlot(vars[a], Slot());
    } else if (code == 'S' || code == 'T') {
      Slot value;
      if (code == 'S') {
        value = vars[c];
      } else {
        value.weak = getObj(vars[c]);
      }

      std::shared_ptr<Obj> obj = getObj(vars[a]);
      if (obj != nullptr && b < obj->parts.size()) {
        Slot old = obj->parts[b];
        obj->parts[b] = value;
        obj = nullptr;
      }
    } else if (code == 'F') {
      Slot value;
      std::shared_ptr<Obj> obj = getObj(vars[b]);
      if (obj != nullptr && c < obj->parts.size()) {
        value = obj->parts[c];
      }
      obj = nullptr;
      setSlot(vars[a], value);
    } else if (code == 'Q') {
      logs.push_back("u" + std::to_string(useAmount(vars[a])));
    }
  }

  for (int i = nvars - 1; i >= 0; i--) {
    setSlot(vars[i], Slot());
  }

  for (int i = 0; i < allObj.size(); i++) {
    if (!allObj[i].expired()) {
      logs.push_back("leak" + std::to_string(i));
    }
  }

  return logs;
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
