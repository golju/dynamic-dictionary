#ifndef DYNAMIC_DICTIONARY_SERIALIZATION_H
#define DYNAMIC_DICTIONARY_SERIALIZATION_H
#include "dictionary.hpp"

namespace dynamic_dictionary
{
template<class T>
inline void dict_t::put_to([[maybe_unused]] T &el) const {}

template<class T>
void dict_t::put_to(std::vector<T> &vec) const
{
  vec.resize(dict_.size());
  for (size_t i = 0; i < dict_.size(); ++i) {
    if (dict_.at(std::to_string(i)).is<dict_t>()) {
      dict_t &tmp_dict = dict_.at(std::to_string(i)).get<dict_t>();
      tmp_dict.put_to(vec[i]);
    } else {
      vec[i] = dict_.at(std::to_string(i)).get_value<T>();
    }
  }
}

template<class T>
void dict_t::put_to(std::map<std::string, T> &m) const
{
  for (auto &el : dict_) {
    if (el.second.is<dict_t>()) {
      dict_t &tmp_dict = el.second.get<dict_t>();
      tmp_dict.put_to(m[el.first]);

    } else {
      m[el.first] = el.second.get_value<T>();
    }
  }
}

template<class T>
inline void write(dict_t &dict, T &&t)
{
  dict_t d(std::forward<T>(t));
  dict.swap(d);
}

template<class T>
inline void read(const dict_t &dict, T &t)
{
  dict.put_to(t);
}
} // dynamic_dictionary

#endif //DYNAMIC_DICTIONARY_SERIALIZATION_H