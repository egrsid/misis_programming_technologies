#include "json_io.hpp"

struct Resources { int64_t cpu, ram, disk; };

struct Result {
    Resources sum;
    std::string order;
    bool fits;
};

static std::vector<Result> solve(const std::vector<Resources>& left, const std::vector<Resources>& right) {
    // TODO: Реализуйте Resources, сложение и частичный порядок; unordered не равен equal.
    return {};
}

int main() {
    std::ios::sync_with_stdio(false);
    const std::string raw(std::istreambuf_iterator<char>(std::cin), {});
    auto read = [&](std::string_view key) {
        auto c = at(raw, key);
        std::vector<Resources> values;
        c.arr([&] {
            std::array<int64_t, 3> v{};
            size_t i = 0;
            c.arr([&] { v.at(i++) = c.num(); });
            values.push_back({v[0], v[1], v[2]});
        });
        return values;
    };
    const auto out = solve(read("left"), read("right"));
    std::cout << '[' << std::boolalpha;
    for (size_t i = 0; i < out.size(); ++i) {
        if (i) std::cout << ',';
        const auto& r = out[i];
        std::cout << "{\"sum\":[" << r.sum.cpu << ',' << r.sum.ram << ',' << r.sum.disk
                  << "],\"order\":\"" << r.order << "\",\"fits\":" << r.fits << '}';
    }
    std::cout << "]\n";
}
