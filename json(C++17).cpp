#include <iostream>
#include<variant>
#include<vector>
#include<map>
#include<optional>
#include<string>
#include<fstream>
#include <sstream>
namespace json {
    
    struct Node;
    using Null = std::monostate;
    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using String = std::string;
    using Array = std::vector<Node>;
    using Object = std::map<std::string, Node>;
    using Value = std::variant<Null, Bool, Int, Float, String, Array, Object>;
    struct Node {
        Value value;
        Node(Value _value) : value(_value) {}
        Node() : value(Null{}) {}
        auto& operator[](const std::string& key) {
            if (auto object = std::get_if<Object>(&value)) {
                return  (*object)[key];
            }
            throw std::runtime_error("not an object");
        }
        auto operator[](size_t index) {
            if (auto array = std::get_if<Array>(&value)) {
                return array->at(index);
            }
            throw std::runtime_error("not an array");
        }
        void push(const Node& rhs) {
            if (auto array = std::get_if<Array>(&value)) {
                array->push_back(rhs);
            }
        }
    };

    struct JsonParser {
        std::string_view json_str;
        size_t pos = 0;

        void parse_whitespace() {
            while (pos < json_str.size() && std::isspace(json_str[pos])) {
                ++pos;
            }
        }

        auto parse_null() -> std::optional<Value> {
            if (json_str.substr(pos, 4) == "null") {
                pos += 4;
                return Null{};
            }
            return{};
        }

        auto parse_true() -> std::optional<Value> {
            if (json_str.substr(pos, 4) == "true") {
                pos += 4;
                return true;
            }
            return {};
        }

        auto parse_false() -> std::optional<Value> {
            if (json_str.substr(pos, 5) == "false") {
                pos += 5;
                return false;
            }
            return {};
        }

        auto parse_number()->std::optional<Value> {
            size_t endpos = pos;
            while (endpos < json_str.size() && (
                std::isdigit(json_str[endpos]) ||
                json_str[endpos] == 'e' ||
                json_str[endpos] == '.')) {
                endpos++;
            }
            std::string number = std::string{ json_str.substr(pos, endpos - pos) };
            pos = endpos;
            static auto is_Float = [](std::string& number) {
                return number.find('.') != number.npos ||
                    number.find('e') != number.npos;
            };
            if (is_Float(number)) {
                try {
                    Float ret = std::stod(number);
                    return ret;
                }
                catch (...) {
                    return {};
                }
            }
            else {
                try {
                    Int ret = std::stoi(number);
                    return ret;
                }
                catch (...) {
                    return {};
                }
            }
        }

        auto parse_string()->std::optional<Value> {
            pos++;// "
            size_t endpos = pos;
            while (pos < json_str.size() && json_str[endpos] != '"') {
                endpos++;
            }
            std::string str = std::string{ json_str.substr(pos, endpos - pos) };
            pos = endpos + 1;// "
            return str;
        }
        auto parse_array()->std::optional<Value> {
            pos++;// [
            Array arr;
            while (pos < json_str.size() && json_str[pos] != ']') {
                auto value = parse_value();
                arr.push_back(value.value());
                parse_whitespace();
                if (pos < json_str.size() && json_str[pos] == ',') {
                    pos++;// ,
                }
                parse_whitespace();
            }
            pos++;// ]
            return arr;
        }

        auto parse_object() ->std::optional<Value> {
            pos++;// {
            Object obj;
            while (pos < json_str.size() && json_str[pos] != '}') {
                auto key = parse_value();
                parse_whitespace();
                if (!std::holds_alternative<String>(key.value())) {
                    return {};
                }
                if (pos < json_str.size() && json_str[pos] == ':') {
                    pos++;// ,
                }
                parse_whitespace();
                auto val = parse_value();
                obj[std::get<String>(key.value())] = val.value();
                parse_whitespace();
                if (pos < json_str.size() && json_str[pos] == ',') {
                    pos++;// ,
                }
                parse_whitespace();
            }
            pos++;// }
            return obj;

        }

        auto parse_value() ->std::optional<Value> {
            parse_whitespace();
            switch (json_str[pos]) {
            case 'n':
                return parse_null();
            case 't':
                return parse_true();
            case 'f':
                return parse_false();
            case '"':
                return parse_string();
            case '[':
                return parse_array();
            case '{':
                return parse_object();
            default:
                return parse_number();
            }
        }

        auto parse() ->std::optional<Node> {
            parse_whitespace();
            auto value = parse_value();
            if (!value) {
                return {};
            }
            return Node{ *value };
        }
    };


    auto parser(std::string_view json_str) ->std::optional<Node> {
        JsonParser p{ json_str };
        return p.parse();
    }


    class JsonGenerator {
    public:
        static auto generate(const Node& node) -> std::string {
            return std::visit(
                [](auto&& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, Null>) {
                        return "null";
                    }
                    else if constexpr (std::is_same_v<T, Bool>) {
                        return arg ? "true" : "false";
                    }
                    else if constexpr (std::is_same_v<T, Int>) {
                        return std::to_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, Float>) {
                        return std::to_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, String>) {
                        return generate_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, Array>) {
                        return generate_array(arg);
                    }
                    else if constexpr (std::is_same_v<T, Object>) {
                        return generate_object(arg);
                    }
                },
                node.value);
        }
        static auto generate_string(const String& str) -> std::string {
            std::string json_str = "\"";
            json_str += str;
            json_str += '"';
            return json_str;
        }
        static auto generate_array(const Array& array) -> std::string {
            std::string json_str = "[";
            for (const auto& node : array) {
                json_str += generate(node);
                json_str += ',';
            }
            if (!array.empty()) json_str.pop_back();
            json_str += ']';
            return json_str;
        }
        static auto generate_object(const Object& object) -> std::string {
            std::string json_str = "{";
            for (const auto& [key, node] : object) {
                json_str += generate_string(key);
                json_str += ':';
                json_str += generate(node);
                json_str += ',';
            }
            if (!object.empty()) json_str.pop_back();
            json_str += '}';
            return json_str;
        }
    };

    inline auto generate(const Node& node) -> std::string { return JsonGenerator::generate(node); }


    auto  operator << (std::ostream& out, const Node& t) ->std::ostream& {
        out << JsonGenerator::generate(t);
        return out;
    }

    
}
using namespace json;


int main() {
    std::ifstream fin("json.txt");
    std::stringstream ss; ss << fin.rdbuf();
    std::string s{ ss.str() };
    auto x = parser(s).value();
    std::cout << x << "\n";
    x.push({ true });
    x.push({ Null {} });
    x["version"] = { 114514LL };
    std::cout << x << "\n";
    std::is_same_v<int, int>;
}