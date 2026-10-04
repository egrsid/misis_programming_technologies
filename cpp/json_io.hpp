#pragma once
#include <algorithm>
#include <array>
#include <compare>
#include <memory>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>
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

