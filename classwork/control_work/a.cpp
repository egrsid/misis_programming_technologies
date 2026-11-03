#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

std::vector<std::pair<std::string, size_t>> top_words(const std::vector<std::string>& words, size_t K) {
    std::unordered_map<std::string, size_t> counts;
    counts.reserve(words.size());

    for (const auto& w: words) {
        ++counts[w];
    }

    std::vector<std::pair<std::string, size_t>> sorted(counts.begin(), counts.end());
    K = std::min(K, sorted.size());

    std::partial_sort(sorted.begin(), sorted.begin() + K, sorted.end(),
                      [](const auto& a, const auto& b) {
                          if (a.second != b.second) { return a.second > b.second; }
                          return a.first < b.first;
                      });

    sorted.resize(K);
    return sorted;
}

int main() 
{
	size_t K;
    std::cin >> K;

    std::vector<std::string> words;
    std::string str;
    while (std::cin >> str) {
        words.push_back(std::move(str));
    }

    for (const auto& [w, c] : top_words(words, K)) {
        std::cout << w << ' ' << c << '\n';
    }

	return 0;
}