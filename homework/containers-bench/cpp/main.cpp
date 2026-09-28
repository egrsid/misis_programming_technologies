#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <deque>

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
    std::string str() {
        ws();
        eat('"');
        const size_t from = p;
        while (p < s.size() && s[p] != '"') ++p;
        return std::string(s.substr(from, p++ - from));
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

// ---------- Генератор splitmix64 (общий для всех языков) ----------
struct Rng {
    uint64_t state;
    uint64_t next() {
        state += 0x9E3779B97F4A7C15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }
    uint64_t rnd() { return next() % 1'000'000; }
};

// ---------- Решение ----------
struct Result { uint64_t len, checksum; };

// TODO: container ∈ {array, linked, deque} → std::vector / std::list / std::deque,
// workload ∈ {push_back, push_front, queue, sorted_insert} — см. README.
// checksum = Σ (i + 1) · a[i] mod 1 000 000 007.
template <typename Cont>
Result use_cont(const std::string& workload, uint64_t n, uint64_t seed) {
    Rng rng{seed};
    Cont cont;

    if (workload == "push_back") {
        for (int i = 0; i < n; ++i) {
            cont.push_back(rng.rnd());
        }
    } else if (workload == "push_front") {
        for (int i = 0; i < n; ++i) {
            if constexpr (std::is_same_v<Cont, std::vector<uint64_t>>) {
                cont.insert(cont.begin(), rng.rnd());
            } else {
                cont.push_front(rng.rnd());
            }
        }
    } else if (workload == "queue") {
        for (int i = 0; i < n; ++i) {
            cont.push_back(rng.rnd());

            if (rng.next() % 3 == 0 && !cont.empty()) {
                if constexpr (std::is_same_v<Cont, std::vector<uint64_t>>) {
                    cont.erase(cont.begin());
                }
            else cont.pop_front();
        }
        }
    } else if (workload == "sorted_insert") {
        for (int i = 0; i < n; ++i) {
            uint64_t x = rng.rnd();
            bool inserted = false;
            for (auto it = cont.begin(); it != cont.end(); ++it) {
                if (*it > x) {
                    cont.insert(it, x);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) cont.push_back(x);
        }

        for (int i = 0; i < n/2; ++i) {
            uint64_t p = rng.next() % cont.size();
            auto it = cont.begin();
            for (int j = 0; j < p; ++j) {
                ++it;
            }
            cont.erase(it);
        }
    }

    uint64_t checksum = 0;
    uint64_t idx = 0;
    for (const auto& val : cont) {
        checksum = (checksum + (idx + 1) * (val % 1'000'000'007)) % 1'000'000'007;
        ++idx;
    }

    return Result{cont.size(), checksum};
}


static Result run(const std::string& container, const std::string& workload, uint64_t n, uint64_t seed) {
    if (container == "array") {
        return use_cont<std::vector<uint64_t>>(workload, n, seed);
    } else if (container == "linked") {
        return use_cont<std::list<uint64_t>>(workload, n, seed);
    } else if (container == "deque") {
        return use_cont<std::deque<uint64_t>>(workload, n, seed);
    }

    return Result{0, 0};
}


int main() {
    std::ios::sync_with_stdio(false);
    std::string line(std::istreambuf_iterator<char>(std::cin), {});
    const std::string container = at(line, "container").str();
    const std::string workload = at(line, "workload").str();
    const auto n = static_cast<uint64_t>(at(line, "n").num());
    const auto seed = static_cast<uint64_t>(at(line, "seed").num());
    const Result res = run(container, workload, n, seed);
    std::cout << "{\"len\": " << res.len << ", \"checksum\": " << res.checksum << "}";
    return 0;
}
