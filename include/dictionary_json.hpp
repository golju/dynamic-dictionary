#ifndef DYNAMIC_DICTIONARY_JSON_H
#define DYNAMIC_DICTIONARY_JSON_H

#include "dictionary.hpp"

#include "../third-party/nlohmann/json.hpp"

namespace dynamic_dictionary
{
inline void traverse_dict(const dict_t &dict, nlohmann::json &j)
{
  auto d = dict.get_dict();
  for (auto &el : d) {
    if (el.second.is<dict_t>()) {
      nlohmann::json sub_j;
      traverse_dict(el.second.get<dict_t>(), sub_j);
      j[el.first] = sub_j;
    } else {
      // int, unsigned int, signed int
      if (el.second.is<int>()) {
        j[el.first] = el.second.get<int>();
      } else if (el.second.is<unsigned int>()) {
        j[el.first] = int(el.second.get<unsigned int>());
      }
      // short, unsigned short, signed short
      else if (el.second.is<short>()) {
        j[el.first] = int(el.second.get<short>());
      } else if (el.second.is<unsigned short>()) {
        j[el.first] = int(el.second.get<unsigned short>());
      }
      // long, unsigned long, signed long
      else if (el.second.is<long>()) {
        j[el.first] = int(el.second.get<long>());
      } else if (el.second.is<unsigned long>()) {
        j[el.first] = int(el.second.get<unsigned long>());
      }
      // char, unsigned char, signed char
      else if (el.second.is<char>()) {
        j[el.first] = int(el.second.get<char>());
      } else if (el.second.is<signed char>()) {
        j[el.first] = int(el.second.get<signed char>());
      } else if (el.second.is<unsigned char>()) {
        j[el.first] = int(el.second.get<unsigned char>());
      }
      // double, float, long double
      else if (el.second.is<double>()) {
        j[el.first] = el.second.get<double>();
      } else if (el.second.is<float>()) {
        j[el.first] = double(el.second.get<float>());
      } else if (el.second.is<float>()) {
        j[el.first] = double(el.second.get<float>());
      } else if (el.second.is<long double>()) {
        j[el.first] = double(el.second.get<long double>());
      }
      // bool
      else if (el.second.is<bool>()) {
        j[el.first] = el.second.get<bool>();
      } else if (el.second.is<std::string>()) {
        std::string tmp = el.second.get_value<std::string>();
        j[el.first] = el.second.get<std::string>();
      }
    }
  }
}

inline void traverse_json(dict_t &dict, const nlohmann::json &j)
{
  for (nlohmann::json::const_iterator it = j.begin(); it != j.end(); ++it) {
    if (it.value().is_object()) {
      nlohmann::json sub_j = it.value();
      dict_t sub_d;
      traverse_json(sub_d, sub_j);
      std::string k = it.key();
      dict.put(std::move(k), sub_d);
    } else {
      std::string k = it.key();
      if (it.value().is_number_integer()) {
        dict.put(std::move(k), j.at(it.key()).get<int>());
      } else if (it.value().is_number_float()) {
        dict.put(std::move(k), j.at(it.key()).get<double>());
      } else if (it.value().is_boolean()) {
        dict.put(std::move(k), j.at(it.key()).get<bool>());
      } else if (it.value().is_string()) {
        dict.put(std::move(k), j.at(it.key()).get<std::string>());
      }
    }
  }
}

inline void save_to_json(std::ostream &os, dict_t dict)
{
  nlohmann::json j;
  traverse_dict(dict, j);
  os << j.dump();
}

inline bool load_from_json(std::istream &is, dict_t &dict)
{
  std::istreambuf_iterator<char> eos;
  std::string s(std::istreambuf_iterator<char>(is), eos);
  nlohmann::json j = nlohmann::json::parse(s.begin(), s.end());

  traverse_json(dict, j);

  nlohmann::json another_j;
  traverse_dict(dict, another_j);
  return true;
}
} // dynamic_dictionary
#endif //DYNAMIC_DICTIONARY_JSON_H