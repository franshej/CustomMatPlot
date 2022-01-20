#include "scp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718

TEST(random_y_values, RealTime) {
  ADD_PLOT;
  ADD_TIMER(1000);
  parent_component->getBounds();

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = (float)rand() / RAND_MAX;
    }

    PLOT_Y({y_test_data});
  };
}

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> sentenceToVector(const std::string& sentence) {
  if (sentence.size() > 80)
    throw std::invalid_argument("Sentence is more than 80 characters.");

  if (!sentence.size()) throw std::invalid_argument("Sentence is empty.");

  std::istringstream iss(sentence);

  return {std::istream_iterator<std::string>{iss},
          std::istream_iterator<std::string>{}};
};

bool doesSentenceHasNoRepeat(const std::string& sentence) noexcept {
  auto words = sentenceToVector(sentence);
  std::sort(words.begin(), words.end());

  return std::adjacent_find(words.begin(), words.end()) != words.end();
};

int main() {
  try {
    std::string test_sentence = "";

    while (true) {
      std::cout << "Enter line to check for repeat: ";
      std::getline(std::cin, test_sentence);

      if (test_sentence == "quit")
        break;
      else if (doesSentenceHasNoRepeat(test_sentence))
        std::cout << "yes" << std::endl;
      else
        std::cout << "no" << std::endl;
    }
  } catch (const std::invalid_argument& error) {
    std::cout << error.what() << std::endl;
  }

  return 0;
}