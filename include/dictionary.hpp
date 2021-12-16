#ifndef DYNAMIC_DICTIONARY_H
#define DYNAMIC_DICTIONARY_H

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <type_traits>
#include <memory>

namespace dynamic_dictionary
{
// =============================================================================
// ================================= any_t =====================================
// =============================================================================
class bad_any_cast : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class any_t {
public:
  any_t() : ptr_(nullptr) {}

  template<typename T>
  any_t(T const &t) : ptr_(
          std::make_unique<any_t_helper<typename std::decay<T>::type>> (t)) {}

  template<class T>
  any_t(T &&value) :
          ptr_(std::make_unique<any_t_helper <typename std::decay<T>::type>>
          (std::forward<T>(value))) {}

  any_t(any_t const &other) :
          ptr_(other.ptr_ ? other.ptr_->clone() : nullptr) {}

  template<class T>
  any_t(std::vector<T> vec);
  template<class T>
  any_t(std::map<std::string, T> m);
  any_t(std::initializer_list<std::pair<std::string, any_t>> i_list);

  template<class T>
  [[nodiscard]] T &get() const
  {
    if (typeid(T) != ptr_->type_info())
      throw bad_any_cast("Bad any cast: method get");
    return static_cast<any_t_helper <T> * >(ptr_.get())->value_;
  }

  template<class T>
  [[nodiscard]] T get_value() const
  {
    if (typeid(T) != ptr_->type_info())
      throw bad_any_cast("Bad any cast: method get_value");
    return static_cast<any_t_helper <T> * >(ptr_.get())->value_;
  }

  template<class T>
  [[nodiscard]] T *get_ptr() const
  {
    if (typeid(T) != ptr_->type_info())
      return nullptr;
    return &(static_cast<any_t_helper<T>*>(ptr_.get())->value_);
  }

  bool operator==(const any_t &other) const
  {
    return ((ptr_)->compare(*(other.ptr_)));
  }

  template<class T>
  [[nodiscard]] bool is() const
  {
    return typeid(T) == ptr_->type_info();
  }

  any_t &operator=(any_t other)
  {
    other.swap(*this);
    return *this;
  }

  void swap(any_t &other)
  {
    ptr_.swap(other.ptr_);
  }

private:
  struct base_any_t_helper
  {
    virtual ~base_any_t_helper() = default;
    virtual const std::type_info &type_info() const = 0;
    virtual base_any_t_helper *clone() const = 0;
    virtual bool compare(base_any_t_helper &other) const = 0;
  };

  template<typename T>
  struct any_t_helper : base_any_t_helper
  {
    any_t_helper(T const &t) : value_(t) {}

    any_t_helper(T &&t) : value_(std::forward<T>(t)) {}

    [[nodiscard]] const std::type_info &type_info() const override
    {
      return typeid(value_);
    }

    [[nodiscard]] base_any_t_helper *clone() const override
    {
      return new any_t_helper(value_);
    }

    [[nodiscard]] bool compare(base_any_t_helper &other) const override
    {
      if (type_info() == other.type_info()) {
        any_t_helper<T> &tmp = dynamic_cast<any_t_helper<T> &>(other);
        return tmp.value_ == value_;
      }
      return false;
    }

    T value_;
  };

  std::unique_ptr<base_any_t_helper> ptr_;
};

inline void swap(any_t &lhs, any_t &rhs)
{
  lhs.swap(rhs);
}

// =============================================================================
// ================================= dict_t ====================================
// =============================================================================
class no_key_error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class dict_t
{
public:
  dict_t() = default;
  ~dict_t() = default;

  dict_t(const dict_t &other) : dict_(other.dict_) {}

  dict_t(dict_t &&other) : dict_(std::move(other.dict_)) {}

  template<class T>
  dict_t(std::vector<T> vec)
  {
    for (size_t i = 0; i < vec.size(); ++i) {
      dict_[std::to_string(i)] = vec[i];
    }
  }

  template<class T>
  dict_t(std::map<std::string, T> m)
  {
    for (auto &el : m) {
      dict_[el.first] = el.second;
    }
  }

  dict_t(std::initializer_list<std::pair<std::string, any_t>> i_list)
  {
    for (auto &el : i_list) {
      dict_[el.first] = el.second;
    }
  }

  template<class T>
  void put_to(T &el) const;
  template<class T>
  void put_to(std::vector<T> &vec) const;
  template<class T>
  void put_to(std::map<std::string, T> &m) const;

  [[nodiscard]] bool is_dict(const std::string& key) const noexcept
  {
    if (dict_.count(key) == 0) {
      return false;
    }
    return dict_.at(key).is<dict_t>();
  }

  void swap(dict_t &other) noexcept
  {
    dict_.swap(other.dict_);
  }

  dict_t &operator=(dict_t other) noexcept
  {
    other.swap(*this);
    return *this;
  }

  bool operator==(const dict_t &other) const noexcept
  {
    return dict_ == other.dict_;
  }

  bool operator!=(const dict_t &other) noexcept
  {
    return !(*this == other);
  }

  [[nodiscard]] bool empty() const noexcept
  {
    return dict_.empty();
  }

  [[nodiscard]] bool contains(const std::string &key) const noexcept
  {
    return dict_.count(key) > 0;
  }

  void clear() noexcept
  {
    dict_.clear();
  }

  [[nodiscard]] bool remove(const std::string &key) noexcept
  {
    return dict_.erase(key) > 0;
  }

  template<class T>
  [[nodiscard]] T *get_ptr(const std::string &key)
  {
    if (dict_.count(key) == 0) {
      return nullptr;
    }
    return dict_.at(key).get_ptr<T>();
  }

  template<class T>
  [[nodiscard]] const T *get_ptr(const std::string &key) const
  {
    if (dict_.count(key) == 0) {
      return nullptr;
    }
    return dict_.at(key).get_ptr<T>();
  }

  template<class T>
  [[nodiscard]] T &get(const std::string &key)
  {
    if (dict_.count(key) == 0) {
      std::string msg = "No key \"" + key + "\"!";
      throw no_key_error(msg);
    }
    return dict_.at(key).get<T>();
  }

  template<class T>
  [[nodiscard]] const T &get(const std::string &key) const
  {
    if (dict_.count(key) == 0) {
      std::string msg = "No key \"" + key + "\"!";
      throw no_key_error(msg);
    }
    return dict_.at(key).get<T>();
  }

  template<class T>
  bool put(std::string &&key, T &&value)
  {
    if (dict_.count(key) > 0) {
      return false;
    }
    dict_.emplace(std::forward<std::string>(key), std::forward<T>(value));
    return true;
  }

  [[nodiscard]] const std::unordered_map<std::string, any_t> &get_dict() const
  {
    return dict_;
  }

private:
  std::unordered_map<std::string, any_t> dict_;
};

inline any_t::any_t(
        std::initializer_list<std::pair<std::string, any_t>> i_list)
        : any_t(dict_t(i_list)) {}

template<class T>
inline any_t::any_t(std::vector<T> vec) : any_t(dict_t(vec)) {}

template<class T>
inline any_t::any_t(std::map<std::string, T> m) : any_t(dict_t(m)) {}
  
// =============================================================================
// ================================= free functions ============================
// =============================================================================
template<class T>
inline bool put(dict_t &dict, std::string key, T &&value)
{
  return dict.put(std::move(key), std::forward<T>(value));
}

template<class T>
inline T &get(dict_t &dict, const std::string &key)
{
  return dict.get<T>(key);
}

template<class T>
inline const T &get(const dict_t &dict, const std::string &key)
{
  return dict.get<T>(key);
}

template<class T>
inline T *get_ptr(dict_t &dict, const std::string &key)
{
  return dict.get_ptr<T>(key);
}

template<class T>
inline const T *get_ptr(const dict_t &dict, const std::string &key)
{
  return dict.get_ptr<T>(key);
}

inline bool empty(dict_t &dict) noexcept
{
  return dict.empty();
}

inline void clear(dict_t &dict) noexcept
{
  dict.clear();
}

inline bool contains(const dict_t &dict, const std::string& key) noexcept
{
  return dict.contains(key);
}

inline bool remove(dict_t &dict, const std::string& key) noexcept
{
  return dict.remove(key);
}

inline bool is_dict(dict_t &dict, std::string key) noexcept
{
  return dict.is_dict(std::move(key));
}
}

#endif //DYNAMIC_DICTIONARY_H
