# JSON Parser using C++17

This project is a JSON parser implemented in C++17, providing a convenient and efficient way to parse and manipulate JSON data. It utilizes features such as `variant` for managing JSON data types, `string_view` for read-only proxy mode, and `optional` for handling non-input exception handling.

## Features

- **C++17 Standard**: The parser is implemented using modern C++17 features, ensuring a clean and efficient codebase.
- **Variant-based Data Management**: JSON data types are handled using `variant`, providing flexibility and type safety during parsing and manipulation.
- **String View Proxy Mode**: `string_view` is used to enable read-only access to JSON data, optimizing performance and memory usage.
- **Dynamic Modification**: The parser supports dynamic modification of parsed JSON data, allowing you to easily update and modify JSON objects and arrays.
- **JSON Output**: The parser can re-output the modified JSON data in the desired format, ensuring compatibility and adherence to JSON standards.
- **Simple Interface Design**: The interface of the parser is designed to be user-friendly and intuitive, making it easy to integrate into your C++ projects.

## Getting Started

To use the JSON parser in your C++ project, follow these steps:

1. **Include the Required Headers**: Include the necessary headers in your C++ source files:

   ```cpp
   #include <iostream>
   #include <variant>
   #include <vector>
   #include <map>
   #include <optional>
   #include <string>
   #include <fstream>
   #include <sstream>
2. **JSON Parser Implementation**: Copy and paste the JSON parser implementation code into your project:

```cpp
// Your JSON parser implementation code here
```
3. **Parsing JSON**: Use the provided functions to parse JSON data from a string or file and obtain the parsed JSON object:

```cpp
std::ifstream fin("json.txt");
std::stringstream ss;
ss << fin.rdbuf();
std::string s{ ss.str() };
auto x = json::parser(s).value();
```
4. **Manipulating JSON**: Once parsed, you can manipulate the JSON object using the available functions for adding, modifying, or removing elements:

```cpp
x.push({ true });
x.push({ json::Null{} });
x["version"] = { 114514LL };
```
5. **Outputting JSON**: After making the desired modifications, you can re-output the JSON data in the desired format using the provided output functions*:

```cpp
std::cout << x << "\n";
```
**Example Usage
Here's an example usage of the JSON parser**:

```cpp
#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>

namespace json {
    // Your JSON parser implementation code here
}

using namespace json;

int main() {
    std::ifstream fin("json.txt");
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string s{ ss.str() };
    auto x = json::parser(s).value();
    std::cout << x << "\n";
    x.push({ true });
    x.push({ json::Null{} });
    x["version"] = { 114514LL };
    std::cout << x << "\n";
    std::is_same_v<int, int>;

    return 0;
}
```
## For more detailed documentation and examples, please refer to the Documentation folder.
