#include <iostream>
#include <cassert>
#include "lruCache/lruCache.hpp"

void testBasicOperations() {
    // Емкость 2
    LruList cache(2);

    // Добавляем 1 -> 10, 2 -> 20
    cache.put(1, 10);
    cache.put(2, 20);

    assert(cache.get(1) == 10); // 1 становится самым свежим
    assert(cache.get(2) == 20); // 2 становится самым свежим
    assert(cache.get(3) == -1); // ключа нет
}

void testEvictionPolicy() {
    LruList cache(2);

    cache.put(1, 10);
    cache.put(2, 20);

    // Обращаемся к 1, теперь порядок свежести: [1, 2] (2 — кандидат на вылет)
    assert(cache.get(1) == 10);

    // Вставляем 3. Должен вытесниться ключ 2
    cache.put(3, 30);

    assert(cache.get(2) == -1); // 2 был удален
    assert(cache.get(1) == 10); // 1 остался
    assert(cache.get(3) == 30); // 3 доступен
}

void testUpdateExistingKey() {
    LruList cache(2);

    cache.put(1, 10);
    cache.put(2, 20);

    // Обновляем значение ключа 1: он становится самым свежим, емкость не растет
    cache.put(1, 100);

    // Вставляем 3. Должен вытесниться ключ 2, а не 1
    cache.put(3, 30);

    assert(cache.get(1) == 100); // значение обновилось и ключ остался
    assert(cache.get(2) == -1);  // 2 был вытеснен
    assert(cache.get(3) == 30);
}

void testCapacityOne() {
    // Краевой случай: емкость = 1
    LruList cache(1);

    cache.put(1, 10);
    assert(cache.get(1) == 10);

    cache.put(2, 20);           // 1 должен сразу вытесниться
    assert(cache.get(1) == -1);
    assert(cache.get(2) == 20);

    cache.put(2, 200);          // обновление единственного элемента
    assert(cache.get(2) == 200);
}

int main() {
    testBasicOperations();
    testEvictionPolicy();
    testUpdateExistingKey();
    testCapacityOne();

    std::cout << "Все тесты успешно пройдены!" << std::endl;
    return 0;
}