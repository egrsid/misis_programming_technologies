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
static std::string classify(double /*price*/, const std::string& /*description*/) {
    return "other";
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    std::cout << '"' << classify(0.0, field_str(line, "description")) << '"';
    return 0;
}
