#include "json_io.hpp"

class Discount {
public:
    virtual ~Discount() = default;
    virtual int64_t apply(int64_t p) const = 0;
};

class DiscountF final : public Discount {
public:
    DiscountF(int64_t a_) : a(a_) {};
    int64_t apply(int64_t p) const override {
        return p - a >= 0 ? p - a : 0;
    }
private:
    int64_t a;
};

class DiscountP final : public Discount {
public:
    DiscountP(int64_t a_) : a(a_) {};
    int64_t apply(int64_t p) const override {
        return (p * (100 - a)) / 100;
    }
private:
    int64_t a;
};

class DiscountT final : public Discount {
public:
    DiscountT(int64_t a_, int64_t t_) : a(a_), t(t_) {};
    int64_t apply(int64_t p) const override {
        return p >= t ? (p - a >= 0 ? p - a : 0) : p;
    }
private:
    int64_t a;
    int64_t t;
};

std::vector<std::unique_ptr<Discount>> make_discounts(const std::string& codes,
                                        const std::vector<int64_t>& amounts, const std::vector<int64_t>& thresholds) {
    std::vector<std::unique_ptr<Discount>> d(codes.size());

    for (size_t i = 0; i < codes.size(); ++i) {
        switch (codes[i]) {
            case 'F':
                d[i] = std::make_unique<DiscountF>(amounts[i]);
                break;
            case 'P':
                d[i] = std::make_unique<DiscountP>(amounts[i]);
                break;
            case 'T':
                d[i] = std::make_unique<DiscountT>(amounts[i], thresholds[i]);
                break;
        }
    }

    return d;
}

static std::vector<int64_t> solve(const std::vector<int64_t>& prices, const std::string& codes,
                                 const std::vector<int64_t>& amounts, const std::vector<int64_t>& thresholds) {
    // TODO: Создайте три типа Discount с apply и общий конвейер без ветвления по типам.
    std::vector<std::unique_ptr<Discount>> d = make_discounts(codes, amounts, thresholds);
    std::vector<int64_t> ans(prices.size());

    for (size_t i = 0; i < prices.size(); ++i) {
        int64_t cur_price = prices[i];
        for (const auto& code : d) {
            cur_price = code->apply(cur_price);
        }

        ans[i] = cur_price;
    }

    return ans;
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
