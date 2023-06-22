/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <random>
#include <vector>

namespace cmp {

/* Get an vector of random values. **/
template <class ValueType>
std::vector<ValueType> generateUniformRandomVector(const std::size_t length,
                                                   const ValueType min,
                                                   const ValueType max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<ValueType> dist(min, max);

  std::vector<ValueType> retval(length);
  std::generate(std::begin(retval), std::end(retval),
                [&] { return dist(gen); });
  return retval;
};

/* Get vector of sine wave. */
template <class ValueType>
std::vector<ValueType> generateSineWaveVector(const std::size_t length,
                                              ValueType min, ValueType max,
                                              const ValueType num_periods,
                                              ValueType phase = 0) {
  std::vector<ValueType> retval(length);

  auto dx =
      (juce::MathConstants<ValueType>::twoPi * num_periods) / ValueType(length);

  cmp::iota_delta<ValueType>(
      retval.begin(), retval.end(), phase, dx, [&](ValueType x) {
        return min + (((std::sin(x) + ValueType(1.0)) * ValueType(0.5)) *
                      (max - min));
      });

  return retval;
};
}  // namespace cmp
