#include <random>
#include <vector>

namespace cmp {

/* Get an vector of float random values. **/
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

}  // namespace cmp
