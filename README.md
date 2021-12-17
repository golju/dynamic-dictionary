# dynamic-dictionary
**:wrench: Tool for data serialization.** Inspired by the usability of the "dict" data structure in Python.

:magnet: Work with [nlohmann/json](https://github.com/nlohmann/json).

:grey_exclamation: Unordered **collections of arbitrary** key-accessed objects.

:thumbsup: Tested on macOS BigSur 11.4.

## Table of contents 
* [Examples](#examples)
  + [Creation](#creation)
  + [Value access](#value-access)
  + [Serialization and Deserialization](#serialization-and-deserialization)
  + [Work with json](#work-with-json)
* [Run example](#run-example)
* [Run tests](#run-tests)
 
## Examples
### Creation
It can be created like this:

```cpp
dynamic_dictionary::dict_t dictionary = {
                                          {"int_value", 100},
                                          {"double_value", 0.02},
                                          {"bool_value", true},
                                          {"inner_dictionary",
                                            {
                                              {"string_value1", std::string("Excellent!")},
                                              {"string_value2", std::string("Brilliant!")}
                                            }
                                          }
                                        };
```

Or like this:
```cpp
dynamic_dictionary::dict_t dictionary;
dynamic_dictionary::put(dictionary, "int_value", 1001);

dynamic_dictionary::dict_t inner_dictionary;
dynamic_dictionary::put(inner_dictionary, "string_value1", std::string("Excellent!"));
dynamic_dictionary::put(inner_dictionary, "string_value2", std::string("Brilliant!"));
dynamic_dictionary::put(dictionary, "inner_dictionary", inner_dictionary);

/*{
 *  {"int_value", 1001},
 *  {"inner_dictionary",
 *    {
 *      {"string_value1", "Excellent!"},
 *      {"string_value2", "Brilliant!"}
 *    }
 *  }
 *}
 */

```

### Value access
```cpp
dynamic_dictionary::dict_t dictionary = {
                                          {"int_value", 100},
                                          {"double_value", 0.02},
                                          {"bool_value", true},
                                          {"inner_dictionary",
                                            {
                                              {"string_value", std::string("Excellent!")},
                                              {"string_value_2", std::string("Brilliant!")}
                                            }
                                          }
                                    };
                                    
std::cout << "string_value: " 
          << dictionary.get<dynamic_dictionary::dict_t>("inner_dictionary").get<std::string>("string_value") 
          << std::endl;
```
Output:
```
Excellent!
```

### Serialization and Deserialization
:white_check_mark: From `std::vector`:
```cpp
std::vector<std::string> jack_did_it = {"This", "is", "the", "house", "that", "Jack", "built."};
dynamic_dictionary::dict_t dictionary;
dynamic_dictionary::write(dictionary, jack_did_it);
```
In dictionary:
```
{
  "0": "This",
  "1": "is",
  "2": "the",
  "3": "house",
  "4": "that",
  "5": "Jack",
  "6": "built."
}
```
:white_check_mark: From `std::map`:
```cpp
std::map<std::string,std::string> rick_astley =
                                  { {"Never gonna give", "you up"},
                                    {"Never gonna let", "you down"},
                                    {"Never gonna run around", "and desert you"},
                                    {"Never gonna make", "you cry"},
                                    {"Never gonna say","goodbye"},
                                    {"Never gonna tell", "a lie and hurt you"} };
dynamic_dictionary::dict_t dictionary;
dynamic_dictionary::write(dictionary, rick_astley);
```
In dictionary:
```
{
    "Never gonna give": "you up",
    "Never gonna let": "you down",
    "Never gonna make": "you cry",
    "Never gonna run around": "and desert you",
    "Never gonna say": "goodbye",
    "Never gonna tell": "a lie and hurt you"
}
```
:white_check_mark: From composition of `std::vector and std::map`:
```cpp
std::map<std::string,std::vector<double>> constants =
                              { {"Mathematical constants", {3.14, 2.71}},
                              {"Physical constants", {9.78}}};
dynamic_dictionary::dict_t dictionary2;
dynamic_dictionary::write(dictionary2, constants);
```
In dictionary:
```
{
  "Mathematical constants": {
    "0": 3.14,
    "1": 2.71
  },
  "Physical constants": {
    "0": 9.78
  }
}
```
### Work with json
:white_check_mark: From dictionary to json `traverse_dict`:
```cpp
std::map<std::string,std::vector<double>> constants =
                        { {"Mathematical constants", {3.14, 2.71}},
                          {"Physical constants", {9.78}}};
dynamic_dictionary::dict_t dictionary;
dynamic_dictionary::write(dictionary, constants);

nlohmann::json output_json;
dynamic_dictionary::traverse_dict(dictionary, output_json);
```
Now we can save json:
```cpp
std::ofstream output("constants.txt");
output << output_json.dump(4) << std::endl;
```
`constants.txt` file:
```json
{
    "Mathematical constants": {
        "0": 3.14,
        "1": 2.71
    },
    "Physical constants": {
        "0": 9.78
    }
}
```
:white_check_mark: From json to dictionary `traverse_json`:
```cpp
std::ifstream input("config_example.txt"); // file is in the folder "example" of this project 
nlohmann::json json_container;
input >> json_container;
dynamic_dictionary::dict_t dictionary;
dynamic_dictionary::traverse_json(dictionary, json_container);
```
In dictionary:
```
{
    "image_recognition_result": {
        "id": 504,
        "vehicle": "BUS",
        "left_seat": {
            "driver": "OK",
            "belt": "NO",
            "phone": "YES",
            "pos": [100,560,120,120],
            "status": "OK"
        },
        "recognition_status": "OK",
        "right_seat": {
            "driver": "UNDEFINED",
            "belt": "UNDEFINED",
            "phone": "UNDEFINED",
            "pos": [-1,-1,-1,-1],
            "status": "UNDEFINED"
        }
    }
}
```
## Run example
```bash
git clone https://github.com/golju/dynamic-dictionary.git
cd dynamic-dictionary
mkdir build
cd build
cmake ..
make example0
./examples/example0
```
Output:
```json
{
    "image_recognition_result": {
        "id": 504,
        "left_seat": {
            "belt": "NO",
            "driver": "OK",
            "phone": "YES",
            "status": "OK"
        },
        "neural_network_info": {
            "name": "BN-AlexNet",
            "train_data_accuracy": 0.976,
            "train_timestamp": 1639768762,
            "version": "6aaf758"
        },
        "recognition_status": "OK",
        "right_seat": {
            "belt": "UNDEFINED",
            "driver": "UNDEFINED",
            "phone": "UNDEFINED",
            "status": "UNDEFINED"
        },
        "vehicle": "BUS"
    }
}
```
## Run tests
```bash
git clone https://github.com/golju/dynamic-dictionary.git
cd dynamic-dictionary
mkdir build
cd build
cmake ..
make tests
./tests/tests
```
Output:
```cpp
[doctest] doctest version is "2.4.7"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases: 5 | 5 passed | 0 failed | 0 skipped
[doctest] assertions: 8 | 8 passed | 0 failed |
[doctest] Status: SUCCESS!

```
