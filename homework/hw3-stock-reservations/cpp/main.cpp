#include "json_io.hpp"
#include <unordered_map>

struct Output {
    std::string results;
    std::vector<std::array<int64_t, 3>> snapshots;
};


class Warehouse {
public:
    Warehouse() = default;
    ~Warehouse() = default;
    Warehouse(const Warehouse&) = default;
    Warehouse(Warehouse&&) = default;

    Warehouse(int64_t initial) : stock(initial), reserved{}, available(initial) {};

    Warehouse& operator=(const Warehouse&) = default;
    Warehouse& operator=(Warehouse&&) = default;

    bool restock(int64_t count);
    bool reserve(int64_t id, int64_t count);
    bool confirm(int64_t id);
    bool cancel(int64_t id);
    std::array<int64_t, 3> snapshot() const;
private:
    int64_t stock = 0;
    int64_t reserved = 0;
    int64_t available = 0;
    std::unordered_map<int64_t, int64_t> reservations;
};

bool Warehouse::restock(int64_t count) {
    if (count <= 0) return false;

    stock += count;
    available += count;

    return true;
}

bool Warehouse::reserve(int64_t id, int64_t count) {
    if(count <= 0 || reservations.contains(id) || available - count < 0) return false;

    reservations[id] = count;
    reserved += count;
    available = stock - reserved;

    return true;
}

bool Warehouse::confirm(int64_t id) {
    if (!reservations.contains(id) || reservations[id] == -1) return false;

    stock -= reservations[id];
    reserved -= reservations[id];
    available = stock - reserved;
    reservations[id] = -1;

    return true;
}

bool Warehouse::cancel(int64_t id) {
    if (!reservations.contains(id)  || reservations[id] == -1) return false;

    reserved -= reservations[id];
    available = stock - reserved;
    reservations[id] = -1;

    return true;
}

std::array<int64_t, 3> Warehouse::snapshot() const {
    return {stock, reserved, available};
}

static Output solve(int64_t initial, const std::string& ops,
                    const std::vector<int64_t>& ids, const std::vector<int64_t>& counts) {
    // TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
    Warehouse warehouse(initial);
    Output output;
    output.results.reserve(ops.size());

    bool is_valid;
    for (int i = 0; i < ops.size(); ++i) {
        switch (ops[i]) {
            case ('A'):
                is_valid = warehouse.restock(counts[i]);
                break;
            case ('R'):
                is_valid = warehouse.reserve(ids[i], counts[i]);
                break;
            case ('C'):
                is_valid = warehouse.confirm(ids[i]);
                break;
            case ('X'):
                is_valid = warehouse.cancel(ids[i]);
                break;
            case ('S'):
                output.snapshots.push_back(warehouse.snapshot());
                break;
        }
        if (ops[i] == 'S') output.results += '=';
        else output.results += is_valid ? '+' : '-';
    }

    return output;
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
