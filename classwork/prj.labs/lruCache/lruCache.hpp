#include <list>
#include <unordered_map>
#include <iostream>
#include <string>

class LruList {
public:
    LruList() = default;
    LruList(int capacity);

    void put(int key, int value);
    int get(int key);
private:
    std::list<std::pair<int, int>> cache_list;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_map;
    int capacity_;
};

inline
LruList::LruList(int capacity) : capacity_(capacity) {}

inline
int LruList::get(int key) {
    auto it = cache_map.find(key);
    if (it == cache_map.end()) return -1;
    auto list_it = it->second;
    cache_list.splice(cache_list.begin(), cache_list, list_it);

    return list_it->second;
}

inline
void LruList::put(int key, int value) {
    if (cache_list.size() >= capacity_) {
        int last_idx = cache_list.back().first;
        cache_map.erase(last_idx);
        cache_list.pop_back();
    }

    auto it = cache_map.find(key);
    if (it != cache_map.end()) {
        auto list_it = it->second;
        list_it->second = value;
        cache_list.splice(cache_list.begin(), cache_list, list_it);
    } else {
        cache_list.emplace_front(key, value);
        cache_map[key] = cache_list.begin();
    }
}