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
#include <chrono>

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

struct Result { uint64_t len, checksum; };

template <typename Cont>
Result use_cont(const std::string& workload, uint64_t n, uint64_t seed, bool with_reserve=false) {
    Rng rng{seed};
    Cont cont;

    if (workload == "push_back") {
        if constexpr (std::is_same_v<Cont, std::vector<uint64_t>>) {
            if (with_reserve) cont.reserve(n);
        }
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

template <typename Cont>
double count_median(const std::string& workload, uint64_t n, bool use_reserve=false) {
    std::vector<uint64_t> seeds = {1, 2, 3, 4, 5};
    std::vector<double> times;

    for (int i = 0; i < seeds.size(); ++i) {
        auto time_before = std::chrono::steady_clock::now();
        use_cont<Cont>(workload, n, seeds[i], use_reserve);
        auto time_after = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> ms = time_after - time_before;
        times.push_back(ms.count());
    }

    std::sort(times.begin(), times.end());
    return times[2];
}

int main() {
    // для сценариев О(n)
    std::vector<uint64_t> n_fast = {10'000, 50'000, 100'000, 500'000};
    // для сценариев О(n**2)
    std::vector<uint64_t> n_slow = {1'000, 5'000, 10'000, 50'000};

    std::cout << "push_back" << std::endl;
    for (auto n : n_fast) {
        double t_vec = count_median<std::vector<uint64_t>>("push_back", n, false);
        double t_vec_reserved = count_median<std::vector<uint64_t>>("push_back", n, true);
        double t_lst = count_median<std::list<uint64_t>>("push_back", n, false);
        double t_deq = count_median<std::deque<uint64_t>>("push_back", n, false);
        std::cout << "n=" << n << " | array: " << t_vec << " ms | array(reserve): " 
                  << t_vec_reserved << " ms | list: " << t_lst << " ms | deque: " << t_deq << " ms" << std::endl;
    }

    std::cout << "push_front" << std::endl;
    for (auto n : n_slow) {
        double t_vec = count_median<std::vector<uint64_t>>("push_front", n);
        double t_lst = count_median<std::list<uint64_t>>("push_front", n);
        double t_deq = count_median<std::deque<uint64_t>>("push_front", n);
        std::cout << "n=" << n << " | vector: " << t_vec << " ms | list: " 
                  << t_lst << " ms | deque: " << t_deq << " ms\n";
    }

    std::cout << "queue" << std::endl;
    for (auto n : n_slow) {
        double t_vec = count_median<std::vector<uint64_t>>("queue", n);
        double t_lst = count_median<std::list<uint64_t>>("queue", n);
        double t_deq = count_median<std::deque<uint64_t>>("queue", n);
        std::cout << "n=" << n << " | vector: " << t_vec << " ms | list: " 
                  << t_lst << " ms | deque: " << t_deq << " ms\n";
    }

    std::cout << "sorted_insert" << std::endl;
    for (auto n : n_slow) {
        double t_vec = count_median<std::vector<uint64_t>>("sorted_insert", n);
        double t_lst = count_median<std::list<uint64_t>>("sorted_insert", n);
        std::cout << "n=" << n << " | vector: " << t_vec << " ms | list: " << t_lst << " ms\n";
    }
}