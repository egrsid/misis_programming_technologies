#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// ---------- Минимальный разбор JSON под формат задачи ----------
struct Cursor {
    std::string_view s;
    size_t p;
    void ws() { while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p]))) ++p; }
    bool eat(char c) { ws(); if (p < s.size() && s[p] == c) { ++p; return true; } return false; }
    int64_t num() {
        ws();
        const bool neg = eat('-');
        int64_t v = 0;
        while (p < s.size() && std::isdigit(static_cast<unsigned char>(s[p]))) v = v * 10 + (s[p++] - '0');
        return neg ? -v : v;
    }
    template <class F> void arr(F f) { eat('['); if (eat(']')) return; do f(); while (eat(',')); eat(']'); }
};

// Курсор сразу после `"key":`.
static Cursor at(std::string_view s, std::string_view key) {
    const std::string k = "\"" + std::string(key) + "\"";
    for (size_t pos = s.find(k); pos != std::string::npos; pos = s.find(k, pos + 1)) {
        Cursor c{s, pos + k.size()};
        if (c.eat(':')) return c;
    }
    return Cursor{s, s.size()};
}

// ---------- Решение ----------
struct Rule { int64_t k, w; };

// TODO: для каждого запроса вернуть 'A' (пропустить) или 'D' (отклонить).
// Нужна амортизированная O(1) на запрос при фиксированном числе правил.
static std::string solve(const std::vector<Rule>& rules, const std::vector<int64_t>& users,
                         const std::vector<int64_t>& times) {
    (void)rules;
    (void)times;
    return std::string(users.size(), 'A');
}

int main() {
    std::ios::sync_with_stdio(false);
    std::string line(std::istreambuf_iterator<char>(std::cin), {});
    std::vector<Rule> rules;
    std::vector<int64_t> users, times;
    Cursor c = at(line, "rules");
    c.arr([&] { Rule r{}; int i = 0; c.arr([&] { (i++ == 0 ? r.k : r.w) = c.num(); }); rules.push_back(r); });
    c = at(line, "users");
    c.arr([&] { users.push_back(c.num()); });
    c = at(line, "times");
    c.arr([&] { times.push_back(c.num()); });
    std::cout << '"' << solve(rules, users, times) << '"';
    return 0;
}
