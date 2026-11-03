#include <algorithm>
#include <charconv>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;
    virtual long long area2() const = 0;  // удвоенная площадь: так она всегда целая
    virtual std::string name() const = 0;
};

// TODO: реализуйте классы Rect(w, h), Square(a), Triangle(base, height).
//   Square должен быть наследником Rect. name() возвращает "Rect", "Square", "Triangle".
//   area2() - удвоенная площадь (у Triangle это base * height).

class Rect : public Shape {
public:
    Rect(long long w, long long h) : w_(w), h_(h) {}
    long long area2() const override { return 2 * w_ * h_; }
    std::string name() const override { return "Rect"; }

private:
    long long w_;
    long long h_;
};

class Square : public Rect {
public:
    explicit Square(long long a) : Rect(a, a) {}
    std::string name() const override { return "Square"; }
};

class Triangle : public Shape {
public:
    Triangle(long long base, long long height) : base_(base), height_(height) {}
    long long area2() const override { return base_ * height_; }
    std::string name() const override { return "Triangle"; }

private:
    long long base_;
    long long height_;
};

// TODO: оператор сравнения по площади, который использует main.
bool operator<(const Shape& a, const Shape& b) { return a.area2() < b.area2(); }

// Создаёт фигуру по названию и параметрам. Верните nullptr, если название неизвестно,
// число параметров не подходит или любой параметр вне диапазона [1, 1000000].
// Rect - 2 параметра, Square - 1, Triangle - 2.
std::unique_ptr<Shape> make_shape(const std::string& kind, const std::vector<long long>& args) {
    for (long long arg : args) {
        if (arg < 1 || arg > 1000000) return nullptr;
    }

    if (kind == "Rect" && args.size() == 2) {
        return std::make_unique<Rect>(args[0], args[1]);
    } else if (kind == "Square" && args.size() == 1) {
        return std::make_unique<Square>(args[0]);
    } else if (kind == "Triangle" && args.size() == 2) {
        return std::make_unique<Triangle>(args[0], args[1]);
    }

    return nullptr;
}

std::string format_area(long long area2) {
    return std::to_string(area2 / 2) + (area2 % 2 ? ".5" : "");
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    std::size_t invalid = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream ss(line);
        std::string kind, token;
        if (!(ss >> kind)) continue;

        std::vector<long long> args;
        bool ok = true;
        while (ss >> token) {
            long long v = 0;
            const char* end = token.data() + token.size();
            auto [ptr, ec] = std::from_chars(token.data(), end, v);
            if (ec != std::errc() || ptr != end) {
                ok = false;
                break;
            }
            args.push_back(v);
        }

        auto shape = ok ? make_shape(kind, args) : nullptr;
        if (shape) {
            shapes.push_back(std::move(shape));
        } else {
            ++invalid;
        }
    }

    long long total2 = 0;
    for (const auto& s : shapes) total2 += s->area2();

    std::cout << "Valid: " << shapes.size() << '\n';
    std::cout << "Invalid: " << invalid << '\n';
    std::cout << "Total: " << format_area(total2) << '\n';
    if (shapes.empty()) {
        std::cout << "Largest: none\n";
    } else {
        auto best = std::max_element(shapes.begin(), shapes.end(),
                                     [](const auto& a, const auto& b) { return *a < *b; });
        std::cout << "Largest: " << (*best)->name() << ' ' << format_area((*best)->area2()) << '\n';
    }
}
