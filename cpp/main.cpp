#include "json_io.hpp"

static std::vector<int64_t> solve(const std::vector<int64_t>& prices, const std::string& codes,
                                 const std::vector<int64_t>& amounts, const std::vector<int64_t>& thresholds) {
    // TODO: Создайте три типа Discount с apply и общий конвейер без ветвления по типам.
    return {};
}

int main() {
    std::ios::sync_with_stdio(false);
    const std::string raw(std::istreambuf_iterator<char>(std::cin), {});
    const auto rules = at(raw, "rules").str();
    std::vector<int64_t> prices, amounts, thresholds;
    auto c = at(raw, "prices"); c.arr([&] { prices.push_back(c.num()); });
    c = at(raw, "amounts"); c.arr([&] { amounts.push_back(c.num()); });
    c = at(raw, "thresholds"); c.arr([&] { thresholds.push_back(c.num()); });
    const auto out = solve(prices, rules, amounts, thresholds);
    std::cout << '[';
    for (size_t i = 0; i < out.size(); ++i) {
        if (i) std::cout << ',';
        std::cout << out[i];
    }
    std::cout << "]\n";
}
