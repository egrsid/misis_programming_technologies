#include <cctype>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <optional>
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

struct ProgramError {
  std::string text;
};

struct Context {
  const Names& fail_close;
  std::vector<std::string>& log;
};

template <class T>
concept Closable = requires(T& res, Context& ctx) {
  { res.close(ctx) } -> std::same_as<std::optional<std::string>>;
};

struct File {
  std::string name;

  std::optional<std::string> close(Context& ctx) {
    if (ctx.fail_close.contains(name)) {
      ctx.log.push_back("close " + name + "!");
      return "close " + name;
    }
    ctx.log.push_back("close " + name);
    return std::nullopt;
  }
};

struct Lock {
  std::string name;

  std::optional<std::string> close(Context& ctx) {
    if (ctx.fail_close.contains(name)) {
      ctx.log.push_back("unlock " + name + "!");
      return "unlock " + name;
    }
    ctx.log.push_back("unlock " + name);
    return std::nullopt;
  }
};

struct Conn {
  std::string name;

  std::optional<std::string> close(Context& ctx) {
    ctx.log.push_back("flush " + name);
    if (ctx.fail_close.contains(name)) {
      ctx.log.push_back("close " + name + "!");
      return "close " + name;
    }
    ctx.log.push_back("close " + name);
    return std::nullopt;
  }
};

using Resource = std::variant<File, Lock, Conn>;

static Resource make_resource(const std::string& type, std::string name) {
  if (type == "file") return File{std::move(name)};
  if (type == "lock") return Lock{std::move(name)};
  return Conn{std::move(name)};
}

static std::optional<std::string> close_resource(Resource& res, Context& ctx) {
  return std::visit([&](Closable auto& item) { return item.close(ctx); }, res);
}

class Scope {
public:
  Scope(Context& ctx, bool is_try, size_t end_pos)
      : ctx_(ctx), is_try_(is_try), end_pos_(end_pos) {}

  Scope(const Scope&) = delete;
  Scope& operator=(const Scope&) = delete;
  Scope(Scope&&) noexcept = default;
  Scope& operator=(Scope&&) = delete;
  ~Scope() noexcept { cleanup(); }

  void add(Resource r) { resources_.push_back(std::move(r)); }

  bool is_try() const noexcept { return is_try_; }
  size_t end_pos() const noexcept { return end_pos_; }

  void exit_normal() {
    auto err = cleanup();
    if (err) {
      throw ProgramError{*err};
    }
  }

private:
  std::optional<std::string> cleanup() noexcept {
    std::optional<std::string> first_err;

    while (!resources_.empty()) {
      Resource r = std::move(resources_.back());
      resources_.pop_back();

      auto err = close_resource(r, ctx_);
      if (err && !first_err) {
        first_err = std::move(err);
      }
    }
    return first_err;
  }

  Context& ctx_;
  std::vector<Resource> resources_;
  bool is_try_;
  size_t end_pos_;
};

static std::vector<std::string> solve(Names fail_open, Names fail_close, Names fail_work,
                                      const std::vector<Op>& ops) {
  std::vector<std::string> log;
  Context ctx{fail_close, log};

  std::vector<size_t> match_end(ops.size(), 0);
  std::vector<size_t> block_stack;

  for (size_t i = 0; i < ops.size(); ++i) {
    const auto& cmd = ops[i][0];
    if (cmd == "begin" || cmd == "try") {
      block_stack.push_back(i);
    } else if (cmd == "end") {
      match_end[block_stack.back()] = i;
      block_stack.pop_back();
    }
  }

  std::vector<Scope> scopes;
  scopes.emplace_back(ctx, false, ops.size());

  size_t ip = 0;

  while (true) {
    try {
      for (; ip < ops.size(); ++ip) {
        const auto& op = ops[ip];
        const std::string& cmd = op[0];

        if (cmd == "open") {
          const std::string& kind = op[1];
          const std::string& name = op[2];

          if (fail_open.contains(name)) {
            log.push_back("open! " + name);
            throw ProgramError{"open " + name};
          }
          log.push_back("open " + name);
          scopes.back().add(make_resource(kind, name));
        } else if (cmd == "work") {
          const std::string& label = op[1];
          if (fail_work.contains(label)) {
            log.push_back("work! " + label);
            throw ProgramError{"work " + label};
          }
          log.push_back("work " + label);
        } else if (cmd == "begin") {
          scopes.emplace_back(ctx, false, match_end[ip]);
        } else if (cmd == "try") {
          scopes.emplace_back(ctx, true, match_end[ip]);
        } else { // "end"
          scopes.back().exit_normal();
          scopes.pop_back();
        }
      }

      scopes.back().exit_normal();
      scopes.pop_back();
      log.push_back("result ok");
      return log;

    } catch (const ProgramError& err) {
      while (true) {
        bool was_try = scopes.back().is_try();
        size_t resume_ip = scopes.back().end_pos();
        scopes.pop_back();

        if (was_try) {
          log.push_back("catch " + err.text);
          ip = resume_ip + 1;
          break;
        }

        if (scopes.empty()) {
          log.push_back("result abort " + err.text);
          return log;
        }
      }
    }
  }
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
