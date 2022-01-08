#pragma once

#if __cpp_constexpr >= 201907L  // Check for a specific version of a feature
#define CONSTEXPR20 \
  constexpr  // This macro should be used in those cases where C++20 only allows
             // it. Example: virtual CONSTEXPR20 int foo() = 0;
#else
#define CONSTEXPR20
#endif

#include <compare>

namespace scp {

/*============================================================================*/

/**< Enum to define the scaling of an axis. */
enum class Scaling : uint32_t {
  linear,     /**< Linear scaling of the graph line. */
  logarithmic /**< Logarithmic scaling of the graph line. */
};

/*============================================================================*/

class GraphLine;
class Grid;
class Frame;
class PlotLabel;
class Legend;
class Trace;
class Zoom;
class PlotLookAndFeel;

/*============================================================================*/

typedef std::vector<std::unique_ptr<GraphLine>> GraphLines;
typedef std::vector<juce::Point<float>> GraphPoints;
typedef std::pair<std::string, juce::Rectangle<int>> Label;
typedef std::vector<Label> LabelVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<juce::Colour> ColourVector;

/*============================================================================*/

/** @brief A struct that defines a single gridline.
 *
 * The struct defines either a vertical or horizontal gridline.
 */
struct GridLine {
  /** Direction of the gridline. */
  enum class Direction { vertical, horizontal } direction;

  /** The x and y position. */
  juce::Point<float> position;

  /** The x or y data/tick value. */
  float tick;

  /** The length of the gridline. */
  float length;
};

/** @brief A template struct that defines min and max. */
template <class ValueType>
struct Lim {
  ValueType min;
  ValueType max;

  constexpr Lim<ValueType>& operator/=(const ValueType val) {
    min /= val;
    max /= val;

    return *this;
  }
};

template <class ValueType>
constexpr Lim<ValueType> operator/(const Lim<ValueType>& rhs,
                                   const ValueType val) {
  Lim<ValueType> new_lim;

  new_lim.min = rhs.min / val;
  new_lim.max = rhs.max / val;

  return move(new_lim);
};

/** @brief A struct that defines min and max using float. */
typedef Lim<float> Lim_f;

/** @brief A struct that defines if a axis labels has been set or not. */
struct IsLabelsSet {
  bool x_label{false};
  bool y_label{false};
  bool title_label{false};
};

/** @brief A view of some common graph attributes. */
struct GraphAttributesView {
  GraphAttributesView(const juce::Rectangle<int>& _graph_bounds,
                      const Lim_f& _x_lim, const Lim_f& _y_lim)
      : graph_bounds{_graph_bounds}, x_lim{_x_lim}, y_lim{_y_lim} {};
  GraphAttributesView(const juce::Rectangle<int>&&, const Lim_f&&,
                      const Lim_f&&) = delete;  // prevents rvalue binding
  const juce::Rectangle<int>& graph_bounds;
  const Lim_f &x_lim, &y_lim;
};

/*============================================================================*/

#define jassert_return(expression, help_text) \
  jassert(expression);                        \
  if (!expression) return;

/*============================================================================*/

template <class value_type>
constexpr std::tuple<value_type, value_type, value_type, value_type>
getRectangleMeasures(juce::Rectangle<int> grid_area) noexcept {
  const auto x = static_cast<value_type>(grid_area.getX());
  const auto y = static_cast<value_type>(grid_area.getY());
  const auto width = static_cast<value_type>(grid_area.getWidth());
  const auto height = static_cast<value_type>(grid_area.getHeight());
  return std::make_tuple(x, y, width, height);
}

template <class value_type>
constexpr auto getLocalBoundsFrom =
    [](const auto& bounds) -> juce::Rectangle<value_type> {
  const auto x = static_cast<value_type>(0);
  const auto y = static_cast<value_type>(0);
  const auto width = static_cast<value_type>(bounds.getWidth());
  const auto height = static_cast<value_type>(bounds.getHeight());
  return juce::Rectangle<value_type>(x, y, width, height);
};

constexpr float getXFromXCoordinate(const float x_pos,
                                    const juce::Rectangle<float>& bounds,
                                    const Lim_f x_lim,
                                    const Scaling x_scaling) noexcept {
  const auto coordinateToXLinear = [&]() {
    const auto x_scale = bounds.getWidth() / (x_lim.max - x_lim.min);
    return ((x_pos - bounds.getX()) / x_scale) + x_lim.min;
  };

  const auto coordinateToXLog = [&]() {
    return powf(10, ((x_pos - bounds.getX()) / bounds.getWidth()) *
                        log10(x_lim.max / x_lim.min)) *
           x_lim.min;
  };

  switch (x_scaling) {
    case Scaling::linear:
      return coordinateToXLinear();
      break;
    case Scaling::logarithmic:
      return coordinateToXLog();
      break;
    default:
      return coordinateToXLinear();
      break;
  }
}

constexpr float getYFromYCoordinate(const float y_pos,
                                    const juce::Rectangle<float>& bounds,
                                    const Lim_f y_lim,
                                    const Scaling y_scaling) noexcept {
  const auto coordinateToYLinear = [&]() {
    const auto y_scale = bounds.getHeight() / std::abs(y_lim.max - y_lim.min);

    return y_lim.max - ((y_pos - bounds.getY()) / y_scale);
  };

  const auto coordinateToYLog = [&]() {
    return powf(10, ((bounds.getHeight() - (y_pos - bounds.getY())) /
                     bounds.getHeight()) *
                        log10(y_lim.max / y_lim.min)) *
           y_lim.min;
  };
  switch (y_scaling) {
    case Scaling::linear:
      return coordinateToYLinear();
      break;
    case Scaling::logarithmic:
      return coordinateToYLog();
      break;
    default:
      return coordinateToYLinear();
      break;
  }
}

constexpr auto getXGraphValueLinear =
    [](const float x, const float x_scale, const float x_offset) -> auto {
  return (x * x_scale) - x_offset;
};

constexpr auto getYGraphValueLinear = [](const float y, const float y_scale,
                                         const float y_offset) -> float {
  return y_offset - (y * y_scale);
};

constexpr auto getXGraphPointsLogarithmic =
    [](const float x, const float x_scale_log, const float x_offset) -> float {
  return (x_scale_log * log10(x)) - x_offset;
};

constexpr auto getYGraphPointsLogarithmic =
    [](const float y, const float y_scale_log, const float y_offset) -> float {
  return y_offset - (y_scale_log * log10(y));
};

constexpr auto getXScaleAndOffset =
    [](const float width, const Lim_f& x_lim,
       const Scaling scaling) -> std::tuple<float, float> {
  float x_scale, x_offset;

  switch (scaling) {
    case Scaling::linear:
      x_scale = width / (x_lim.max - x_lim.min);
      x_offset = x_lim.min * x_scale;
      break;
    case Scaling::logarithmic:
      x_scale = width / log10(x_lim.max / x_lim.min);
      x_offset = x_scale * log10(x_lim.min);
      break;
    default:
      break;
  }

  return {x_scale, x_offset};
};

constexpr auto getYScaleAndOffset =
    [](const float height, const Lim_f& y_lim,
       const Scaling scaling) -> std::tuple<float, float> {
  float y_scale, y_offset;

  switch (scaling) {
    case Scaling::linear:
      y_scale = height / (y_lim.max - y_lim.min);
      y_offset = height + (y_lim.min * y_scale);
      break;
    case Scaling::logarithmic:
      y_scale = height / log10(y_lim.max / y_lim.min);
      y_offset = height + y_scale * log10(y_lim.min);
      break;
    default:
      break;
  }

  return {y_scale, y_offset};
};

/*============================================================================*/

template <class ValueType>
[[nodiscard]] std::pair<std::string, std::string> valueToString(
    const ValueType value, const GraphAttributesView& graph_attributes,
    const bool is_x,
    const std::size_t size_of_exponent_before_factor =
        std::numeric_limits<std::size_t>::max()) {
  static_assert(std::is_same<float, ValueType>::value ||
                    std::is_same<const float, ValueType>::value ||
                    std::is_same<double, ValueType>::value ||
                    std::is_same<const double, ValueType>::value,
                "Type must be either float or double");
  auto lims = is_x ? graph_attributes.x_lim : graph_attributes.y_lim;

  if (lims.min == 0.f && lims.max == 0.f) {
    // Either min or max must be non zero.
    jassertfalse;
  }

  const auto max_exp = lims.max > 0 ? std::log10(abs(lims.max)) : 0;
  const auto min_exp = lims.min > 0 ? std::log10(abs(lims.min)) : 0;

  const auto max_abs_exp = std::ceil(abs(max_exp));
  const auto min_abs_exp = std::ceil(abs(min_exp));

  const auto exp_diff = max_exp - min_exp;

  const auto [largest_exp,
              largest_abs_exp] = [&]() -> std::pair<ValueType, ValueType> {
    if (max_abs_exp > min_abs_exp)
      return {max_exp, max_abs_exp};
    else
      return {min_exp, min_abs_exp};
  }();

  const auto should_factor_out =
      exp_diff < ValueType(size_of_exponent_before_factor) &&
      largest_abs_exp >= ValueType(size_of_exponent_before_factor);

  std::string factor_text = "";
  std::string value_text = "";

  if (should_factor_out) {
    const auto factor = ValueType(std::pow(10.0, largest_abs_exp));

    lims /= factor;

    factor_text = "1e" + std::to_string(int(largest_exp));
    value_text = std::to_string(value / factor);
  } else {
    value_text = std::to_string(value);
  }

  const auto num_digits_diff =
      std::size_t(std::ceil(std::log10(abs(lims.max - lims.min))));

  const auto num_digits_before_sign =
      num_digits_diff > largest_abs_exp ? num_digits_diff : largest_abs_exp;

  const auto num_digits_before_exponent_sign =
      num_digits_before_sign + int(value < 0);

  const auto num_digits_before_checking_ending_character =
      num_digits_before_exponent_sign + 2 * int(largest_exp < 0);

  value_text =
      value_text.substr(0u, num_digits_before_checking_ending_character);

  if (value_text.back() == '.') value_text.pop_back();

  return {value_text, factor_text};
}

template <class float_type>
[[nodiscard]] const std::string convertFloatToString(
    const float_type value, const std::size_t num_decimals,
    const std::size_t max_string_len) {
  if constexpr (!(std::is_same<float, float_type>::value ||
                  std::is_same<const float, float_type>::value ||
                  std::is_same<double, float_type>::value ||
                  std::is_same<const double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto pow_of_ten = value == 0.f ? 0 : int(floor(log10(abs(value))));
  const auto is_neg = std::size_t(value < 0);

  auto text_out = std::to_string(value);

  const auto len_before_dec = pow_of_ten < 0
                                  ? std::size_t(abs(float(pow_of_ten)))
                                  : std::size_t(pow_of_ten) + 1u;
  const auto req_len = len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (max_string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto two_decimals =
          text_out.substr(is_neg + std::size_t(pow_of_ten) + 1u, 3);
      const auto first_digit = text_out.substr(0, 1u + is_neg);
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals = text_out.substr(len_before_dec + is_neg + 1u, 4);
      three_decimals.insert(0, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else {
    text_out = text_out.substr(0, len_before_dec + is_neg + 1u + num_decimals);
  }

  return text_out;
}

const auto getMaximumLabelWidth =
    [](const auto num1, const auto num2, const juce::Font& font,
       const std::size_t maximum_allowed_characters) -> const int {
  const auto min_text = convertFloatToString<decltype(num1)>(
      num1, 2u, maximum_allowed_characters);
  const auto max_text = convertFloatToString<decltype(num2)>(
      num2, 2u, maximum_allowed_characters);
  return std::max(font.getStringWidth(min_text), font.getStringWidth(max_text));
};

/*============================================================================*/

}  // namespace scp
