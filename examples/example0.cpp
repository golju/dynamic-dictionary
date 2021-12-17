#include "dictionary_json.hpp"

#include <fstream>
#include <iostream>

int main() {
  const std::string test_data_folder = TEST_DATA_FOLDER;
  std::string path = test_data_folder + "/config_example.txt";
  std::ifstream input(path);

  nlohmann::json json_container;
  input >> json_container;

  dynamic_dictionary::dict_t dictionary;
  dynamic_dictionary::traverse_json(dictionary, json_container);

  /* Let's imagine that we want to put the neural
   * network info to image recognition result:
   */
  dynamic_dictionary::dict_t neural_network_info =
          {
            {"name", std::string("BN-AlexNet")},
            {"version", std::string("6aaf758")},
            {"train_data_accuracy", 0.976},
            {"train_timestamp", 1639768762}
          };
  dynamic_dictionary::put(dictionary.get<dynamic_dictionary::dict_t>("image_recognition_result"),
                     "neural_network_info", neural_network_info);
  dynamic_dictionary::traverse_dict(dictionary, json_container);
  std::cout << json_container.dump(4) << std::endl;
}