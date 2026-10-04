#include "json_io.hpp"

struct Output {
    std::string results;
    std::vector<std::array<int64_t, 3>> snapshots;
};

static Output solve(int64_t initial, const std::string& ops,
                    const std::vector<int64_t>& ids, const std::vector<int64_t>& counts) {
    // TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
    return {std::string(ops.size(), '?'), {}};
}

int main() {
    std::ios::sync_with_stdio(false);
    const std::string raw(std::istreambuf_iterator<char>(std::cin), {});
    const auto initial = at(raw, "initial").num();
    const auto ops = at(raw, "ops").str();
    std::vector<int64_t> ids, counts;
    auto c = at(raw, "ids");
    c.arr([&] { ids.push_back(c.num()); });
    c = at(raw, "counts");
    c.arr([&] { counts.push_back(c.num()); });
    const auto out = solve(initial, ops, ids, counts);
    std::cout << "{\"results\":\"" << out.results << "\",\"snapshots\":[";
    for (size_t i = 0; i < out.snapshots.size(); ++i) {
        if (i) std::cout << ',';
        const auto& s = out.snapshots[i];
        std::cout << '[' << s[0] << ',' << s[1] << ',' << s[2] << ']';
    }
    std::cout << "]}\n";
}
