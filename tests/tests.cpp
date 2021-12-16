//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "dictionary.hpp"
#include "dictionary_serialization.hpp"
#include "dictionary_json.hpp"

#include <sstream>

dynamic_dictionary::dict_t get_dictionary()
{
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
  return dictionary;
}

TEST_CASE("DynamicDictionary.CopyMoveTests")
{
  dynamic_dictionary::dict_t dictionary = get_dictionary();
  dynamic_dictionary::dict_t dictionary_copy = dictionary;
  CHECK(dictionary_copy == dictionary);

  dynamic_dictionary::dict_t data_thief;
  data_thief = std::move(dictionary);
  CHECK(dictionary.empty());
  CHECK(data_thief == dictionary_copy);
}

TEST_CASE("DynamicDictionary.PutGetTests")
{
  dynamic_dictionary::dict_t dictionary;
  dynamic_dictionary::put(dictionary, "int_value", 1001);

  dynamic_dictionary::dict_t inner_dictionary;
  dynamic_dictionary::put(inner_dictionary, "string_value", std::string("Excellent!"));
  dynamic_dictionary::put(dictionary, "inner_dictionary", inner_dictionary);

  CHECK(dictionary.get<int>("int_value") == 1001);
  CHECK(dictionary.get_ptr<double>("int_value") == nullptr);
  CHECK(dictionary.get<dynamic_dictionary::dict_t>("inner_dictionary")
          .get<std::string>("string_value") == std::string("Excellent!"));
}

using composition_t = std::map<std::string, std::vector<std::map<std::string, double>>>;
composition_t get_composition()
{
  composition_t composition =
                        {
                          {"one",
                            {
                              { },
                              {
                                {"three", 3.0}
                              },
                              {
                                {"four", 4},
                                {"five", 5.0}
                              }
                            }
                          },
                          {"no_value",{}}
                        };

  return composition;
}
TEST_CASE("DynamicDictionary.CompositionTests")
{
  composition_t composition = get_composition();

  dynamic_dictionary::dict_t dictionary;
  dynamic_dictionary::write(dictionary, composition);

  composition_t read_composition;
  read(dictionary, read_composition);

  CHECK(read_composition == composition);

  double deep_value = dictionary.get<dynamic_dictionary::dict_t>("one").
                      get<dynamic_dictionary::dict_t>("1").
                      get<double>("three");

  assert(deep_value == 3);
}

TEST_CASE("DynamicDictionary.InequalityTests")
{
  composition_t composition = get_composition();

  dynamic_dictionary::dict_t dictionary;
  dynamic_dictionary::write(dictionary, composition);

  dynamic_dictionary::dict_t dictionary_copy = dictionary;
  CHECK(dictionary == dictionary_copy);

  double &deep_value = dictionary.get<dynamic_dictionary::dict_t>("one").
          get<dynamic_dictionary::dict_t>("1").
          get<double>("three");
  deep_value += 1.0;

  assert(dictionary != dictionary_copy);
}

TEST_CASE("DynamicDictionary.JsonTests")
{
  dynamic_dictionary::dict_t dictionary = get_dictionary();

  dynamic_dictionary::dict_t another_dictionary;
  write(another_dictionary, dictionary);

  std::stringstream ss;
  dynamic_dictionary::save_to_json(ss, dictionary);
  ss.rdbuf()->pubseekpos(0);

  dynamic_dictionary::dict_t loaded_dictionary;
  dynamic_dictionary::load_from_json(ss, loaded_dictionary);

  assert(dictionary == loaded_dictionary);
}