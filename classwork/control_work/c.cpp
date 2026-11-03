#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <list>

struct ResourceError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// RAII-обёртка над ресурсом: при создании печатает "open NAME", при уничтожении "close NAME".
// Копировать ресурс нельзя. Добавьте нужные поля и методы.
class Resource {
public:
    Resource(std::string name, std::ostream& out) : name_(name), out_(out) {
        out_ << "open " << name_ << '\n';
    }
    ~Resource() { out_ << "close " << name_ << '\n'; }
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    const std::string& name() const;
private:
    std::string name_;
    std::ostream& out_;
};

// Менеджер ресурсов с вложенными областями (scope). Сразу после создания существует
// корневая область. Ресурсы каждой области закрываются в ОБРАТНОМ порядке открытия.
class Manager {
public:
    // limit - максимальное число одновременно открытых ресурсов. Печатать нужно в out.
    Manager(std::size_t limit, std::ostream& out) { scopes_.emplace_back(); }

    // Закрывает всё, что осталось открытым: сначала самая вложенная область, потом внешние.
    ~Manager() {
        while (!scopes_.empty()) {
            close_current();
            scopes_.pop_back();
        }
    }

    // Открывает ресурс в текущей области.
    //   имя уже открыто (в любой области)  -> throw ResourceError("busy NAME")
    //   открыто limit ресурсов              -> throw ResourceError("limit NAME")
    // Проверка busy идёт раньше limit. При исключении состояние менеджера не меняется.
    void open(const std::string& name);

    void begin_scope() { scopes_.emplace_back(); }

    // Закрывает все ресурсы текущей области (в обратном порядке) и выходит из неё.
    void end_scope() {
        close_current();
        scopes_.pop_back();
    }

    // Закрывает все ресурсы текущей области (в обратном порядке), но остаётся в ней.
    void rollback_scope() { close_current(); }

    // Передаёт ресурс из текущей области в родительскую без закрытия (в конец списка родителя).
    //   корневая область (родителя нет)          -> throw ResourceError("no-parent NAME")
    //   ресурса нет в текущей области            -> throw ResourceError("not-owned NAME")
    void move_up(const std::string& name);

private:
    std::vector<std::list<std::unique_ptr<Resource>>> scopes_;
    std::ostream& out_;
    size_t limit;

    void close_current() {
        auto& scope = scopes_.back();
        while (!scope.empty()) {
            scope.pop_back();
        }
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t limit = 0, q = 0;
    std::cin >> limit >> q;

    Manager manager(limit, std::cout);
    while (q-- > 0) {
        std::string cmd, name;
        std::cin >> cmd;
        try {
            if (cmd == "BEGIN") {
                manager.begin_scope();
            } else if (cmd == "END") {
                manager.end_scope();
            } else if (cmd == "OPEN") {
                std::cin >> name;
                manager.open(name);
            } else if (cmd == "MOVE") {
                std::cin >> name;
                manager.move_up(name);
                std::cout << "moved " << name << '\n';
            }
        } catch (const ResourceError& e) {
            std::cout << "error " << e.what() << '\n';
            manager.rollback_scope();
        }
    }
}
