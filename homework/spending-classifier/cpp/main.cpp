#include <iostream>
#include <string>

// "key": "..."  — терпимо к пробелам после ':'.
static std::string field_str(const std::string& line, const std::string& key) {
    const std::string k = "\"" + key + "\"";
    auto kpos = line.find(k);
    if (kpos == std::string::npos) return "";
    auto colon = line.find(':', kpos + k.size());
    if (colon == std::string::npos) return "";
    auto q1 = line.find('"', colon + 1);
    if (q1 == std::string::npos) return "";
    auto q2 = line.find('"', q1 + 1);
    if (q2 == std::string::npos) return "";
    return line.substr(q1 + 1, q2 - q1 - 1);
}

// TODO: electronics / groceries / clothing / transport / other.
static std::string classify(double /*price*/, const std::string& description) {
    if (description.find("хлеб") != std::string::npos ||
        description.find("Хлеб") != std::string::npos ||
        description.find("молоко") != std::string::npos ||
        description.find("Молоко") != std::string::npos ||
        description.find("сыр") != std::string::npos ||
        description.find("Сыр") != std::string::npos ||
        description.find("Пятёрочк") != std::string::npos ||
        description.find("пятёрочк") != std::string::npos ||
        description.find("Магнит") != std::string::npos ||
        description.find("магнит") != std::string::npos ||
        description.find("мясо") != std::string::npos ||
        description.find("Мясо") != std::string::npos ||
        description.find("курица") != std::string::npos ||
        description.find("Курица") != std::string::npos ||
        description.find("рыба") != std::string::npos ||
        description.find("Рыба") != std::string::npos ||
        description.find("яйц") != std::string::npos ||
        description.find("Яйц") != std::string::npos ||
        description.find("продукты") != std::string::npos ||
        description.find("Продукты") != std::string::npos ||
        description.find("продукт") != std::string::npos ||
        description.find("Продукт") != std::string::npos ||
        description.find("супермаркет") != std::string::npos ||
        description.find("Супермаркет") != std::string::npos) {
        return "groceries";
    }

    if (description.find("ноутбук") != std::string::npos ||
        description.find("Ноутбук") != std::string::npos ||
        description.find("смартфон") != std::string::npos ||
        description.find("Смартфон") != std::string::npos ||
        description.find("телефон") != std::string::npos ||
        description.find("Телефон") != std::string::npos ||
        description.find("планшет") != std::string::npos ||
        description.find("Планшет") != std::string::npos ||
        description.find("наушники") != std::string::npos ||
        description.find("Наушники") != std::string::npos ||
        description.find("компьютер") != std::string::npos ||
        description.find("Компьютер") != std::string::npos ||
        description.find("телевизор") != std::string::npos ||
        description.find("Телевизор") != std::string::npos ||
        description.find("Xiaomi") != std::string::npos ||
        description.find("xiaomi") != std::string::npos ||
        description.find("Redmi") != std::string::npos ||
        description.find("redmi") != std::string::npos ||
        description.find("Apple") != std::string::npos ||
        description.find("apple") != std::string::npos ||
        description.find("iPhone") != std::string::npos ||
        description.find("iphone") != std::string::npos ||
        description.find("Samsung") != std::string::npos ||
        description.find("samsung") != std::string::npos ||
        description.find("Huawei") != std::string::npos ||
        description.find("huawei") != std::string::npos ||
        description.find("монитор") != std::string::npos ||
        description.find("Монитор") != std::string::npos) {
        return "electronics";
    }

    if (description.find("куртка") != std::string::npos ||
        description.find("Куртка") != std::string::npos ||
        description.find("джинсы") != std::string::npos ||
        description.find("Джинсы") != std::string::npos ||
        description.find("ботинки") != std::string::npos ||
        description.find("Ботинки") != std::string::npos ||
        description.find("кроссовки") != std::string::npos ||
        description.find("Кроссовки") != std::string::npos ||
        description.find("футболка") != std::string::npos ||
        description.find("Футболка") != std::string::npos ||
        description.find("одежда") != std::string::npos ||
        description.find("Одежда") != std::string::npos ||
        description.find("обувь") != std::string::npos ||
        description.find("Одедь") != std::string::npos ||
        description.find("пальто") != std::string::npos ||
        description.find("Пальто") != std::string::npos) {
        return "clothing";
    }

    if (description.find("такси") != std::string::npos ||
        description.find("Такси") != std::string::npos ||
        description.find("метро") != std::string::npos ||
        description.find("Метро") != std::string::npos ||
        description.find("автобус") != std::string::npos ||
        description.find("Автобус") != std::string::npos ||
        description.find("каршеринг") != std::string::npos ||
        description.find("Каршеринг") != std::string::npos ||
        description.find("поездка") != std::string::npos ||
        description.find("Поездка") != std::string::npos ||
        description.find("билет") != std::string::npos ||
        description.find("Билет") != std::string::npos ||
        description.find("Яндекс") != std::string::npos ||
        description.find("яндекс") != std::string::npos ||
        description.find("бензин") != std::string::npos ||
        description.find("Бензин") != std::string::npos ||
        description.find("топливо") != std::string::npos ||
        description.find("Топливо") != std::string::npos ||
        description.find("заправка") != std::string::npos ||
        description.find("Заправка") != std::string::npos ||
        description.find("АЗС") != std::string::npos ||
        description.find("азс") != std::string::npos ||
        description.find("Лукойл") != std::string::npos ||
        description.find("лукойл") != std::string::npos ||
        description.find("Газпром") != std::string::npos ||
        description.find("газпром") != std::string::npos) {
        return "transport";
    }

    return "other";
}


int main() {
    std::string line;
    std::getline(std::cin, line);
    std::cout << '"' << classify(0.0, field_str(line, "description")) << '"';
    return 0;
}
