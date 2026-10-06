#include "json_io.hpp"

struct Resources { 
    int64_t cpu, ram, disk; 

    std::partial_ordering operator<=>(const Resources& rhs) const {
        if (cpu == rhs.cpu && ram == rhs.ram && disk == rhs.disk) {
            return std::partial_ordering::equivalent;
        }

        if (cpu <= rhs.cpu && ram <= rhs.ram && disk <= rhs.disk) {
            return std::partial_ordering::less;
        }

        if (cpu >= rhs.cpu && ram >= rhs.ram && disk >= rhs.disk) {
            return std::partial_ordering::greater;
        }

        return std::partial_ordering::unordered;
    }

};

bool operator==(const Resources& lhs, const Resources& rhs) { 
        return lhs.cpu == rhs.cpu && lhs.ram == rhs.ram && lhs.disk == rhs.disk;
}

Resources operator+(const Resources& lhs, const Resources& rhs) {
        Resources res{lhs.cpu + rhs.cpu, lhs.ram + rhs.ram, lhs.disk + rhs.disk};
        return res;
    }

struct Result {
    Resources sum;
    std::string order;
    bool fits;
};

static std::vector<Result> solve(const std::vector<Resources>& left, const std::vector<Resources>& right) {
    // TODO: Реализуйте Resources, сложение и частичный порядок; unordered не равен equal.
    std::vector<Result> res(left.size());

    for (size_t i = 0; i < left.size(); ++i) {
        res[i].sum = left[i] + right[i];

        const auto order = left[i] <=> right[i];

        if (order == std::partial_ordering::equivalent) {
            res[i].order = "equal";
            res[i].fits = true;
        } else if (order == std::partial_ordering::less) {
            res[i].order = "less";
            res[i].fits = true;
        } else if (order == std::partial_ordering::greater) {
            res[i].order = "greater";
            res[i].fits = false;
        } else {
            res[i].order = "unordered";
            res[i].fits = false;
        }
    }

    return res;
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
