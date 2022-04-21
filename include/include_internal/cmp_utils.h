/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_utils.h
 *
 * @brief Utility functions, classes strucs etc.
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

namespace cmp {

/*============================================================================*/

/**
 * \struct
 * @breif A struct that defines a single legend label
 */
struct LegendLabel {
  LegendLabel(std::string desciption)
      : description{desciption}, description_colour{juce::Colours::pink} {}

  LegendLabel(std::string desciption, juce::Colour description_colour)
      : description{desciption}, description_colour{description_colour} {}

  LegendLabel()
      : description{"Unknown data serie."},
        description_colour{juce::Colours::pink} {}

  /** The description text of the associated graph_line. */
  std::string description;

  /** The colour of the description text. */
  juce::Colour description_colour;
};

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

/** @brief A struct that defines if a axis labels has been set or not. */
struct IsLabelsSet {
  bool x_label{false};
  bool y_label{false};
  bool title_label{false};
};

/** @brief A view of the data required to draw a graph_line++ */
struct GraphLineDataView {
  GraphLineDataView(const std::vector<float>& x, const std::vector<float>& y,
                    const GraphPoints& gp, const std::vector<std::size_t>& i,
                    const GraphAttribute& ga)
      : x_data{x},
        y_data{y},
        graph_points{gp},
        graph_point_indices{i},
        graph_attribute{ga} {};

  GraphLineDataView(const std::vector<float>&&, const std::vector<float>&&,
                    const GraphPoints&&,
                    const std::vector<std::size_t>&&) =
      delete;  // prevents rvalue binding

  const std::vector<float>&x_data, &y_data;
  const GraphPoints& graph_points;
  const std::vector<std::size_t>& graph_point_indices;
  const GraphAttribute& graph_attribute;
};

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
    const ValueType value, const CommonPlotParameterView common_plot_params,
    const bool is_x,
    [[maybe_unused]] const std::size_t size_of_exponent_before_factor =
        std::numeric_limits<std::size_t>::max()) {
  static_assert(std::is_same<float, ValueType>::value ||
                    std::is_same<const float, ValueType>::value ||
                    std::is_same<double, ValueType>::value ||
                    std::is_same<const double, ValueType>::value,
                "Type must be either float or double");
  auto lims = is_x ? common_plot_params.x_lim : common_plot_params.y_lim;

  const auto max_exp = lims.max != 0 ? std::log10(abs(lims.max)) : 0;
  const auto min_exp = lims.min != 0 ? std::log10(abs(lims.min)) : 0;

  const auto max_abs_exp = std::ceil(abs(max_exp));
  const auto min_abs_exp = std::ceil(abs(min_exp));

  const auto exp_diff = max_exp - min_exp;

  const auto [largest_exp,
              largest_abs_exp] = [&]() -> std::pair<ValueType, ValueType> {
    if (max_abs_exp < 2 && min_abs_exp < 2)
      return {ValueType(2), ValueType(2)};
    else if (max_abs_exp > min_abs_exp)
      return {max_exp, max_abs_exp};
    else
      return {min_exp, min_abs_exp};
  }();

  std::string factor_text = "";
  std::string value_text = "";

  // Not used atm.
  if constexpr (false) {
    const auto factor = ValueType(std::pow(10.0, largest_abs_exp));

    lims /= factor;

    factor_text = "1e" + std::to_string(int(largest_exp));
    value_text = std::to_string(value / factor);
  } else {
    value_text = std::to_string(value);
  }

  const auto lims_diff = lims.max - lims.min;
  const auto lims_diff_log = std::log10(lims_diff);

  auto num_digits_diff =
      lims_diff_log < 0 ? std::abs(lims_diff_log) + 2 : lims_diff_log;

  auto num_digits_before_sign = 0.0f;
  if (lims_diff_log >= 0 && largest_exp >= 0) {
    if (int(std::ceil(std::log10(std::abs(value)))) == int(largest_abs_exp)) {
      num_digits_before_sign += largest_abs_exp + 1.0f;
    } else {
      num_digits_before_sign += largest_abs_exp;
    }
  } else if (lims_diff_log < 0.0f && largest_exp > 0.0f) {
    num_digits_before_sign += largest_abs_exp + num_digits_diff;
  } else if (lims_diff_log < 0.0f && largest_exp < 0.0f) {
    num_digits_before_sign += std::max(num_digits_diff, largest_abs_exp);
  }

  const auto num_digits_before_exponent_sign =
      std::size_t(std::ceil(num_digits_before_sign) + int(value < 0));

  const auto num_digits_before_checking_ending_character =
      std::size_t(num_digits_before_exponent_sign + 2 * int(largest_exp < 0));

  auto value_text_out =
      value_text.substr(0u, num_digits_before_checking_ending_character);

  if (value_text_out.back() == '.') {
    if (lims_diff < 20) {
      value_text_out = value_text.substr(
          0u, num_digits_before_checking_ending_character + 1);
    } else {
      value_text_out = value_text.substr(
          0u, num_digits_before_checking_ending_character - 1);
    }
  }

  return {value_text_out, factor_text};
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

template <class T>
class ParamBase {
 public:
  operator T() const { return m_param; }

 protected:
  bool m_is_set{false};
  T m_param;
};

template <class T>
class ExplicitBoolOperator : public ParamBase<T> {
 public:
  constexpr explicit operator bool() const noexcept { return this->m_is_set; }
};

template <class T>
class ParamVal
    : public std::conditional_t<std::is_same_v<T, bool>, ParamBase<T>,
                                ExplicitBoolOperator<T>> {
 public:
  ParamVal(const T param) {
    this->m_is_set = false;
    this->m_param = param;
  };
  ParamVal(T& param) {
    this->m_is_set = false;
    this->m_param = param;
  };
  ParamVal() {
    this->m_is_set = false;
    this->m_param = T();
  };
  ~ParamVal() = default;

  T& operator=(const T& rhs) {
    if (&this->m_param == &rhs) return this->m_param;
    this->m_param = rhs;
    this->m_is_set = true;
    return this->m_param;
  }
  ParamVal<T> operator=(const ParamVal<T>& rhs) {
    if (this == &rhs) return *this;
    if (rhs) {
      this->m_param = rhs;
      this->m_is_set = true;
    }
    return *this;
  }
};

}  // namespace cmp