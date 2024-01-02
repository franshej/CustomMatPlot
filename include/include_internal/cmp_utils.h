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

  /** Type of grid line. */
  enum class Type { normal, translucent } type{Type::normal};
};

/** @brief A struct that defines if a axis labels has been set or not. */
struct IsLabelsSet {
  bool x_label{false};
  bool y_label{false};
  bool title_label{false};
};

/*============================================================================*/

static GraphLineDataViewList createGraphLineDataViewList(
    const GraphLines& graph_lines) {
  GraphLineDataViewList graph_line_data_view_list;
  graph_line_data_view_list.reserve(graph_lines.size());

  for (const auto& gl : graph_lines) {
    graph_line_data_view_list.emplace_back(GraphLineDataView(*gl));
  }

  return graph_line_data_view_list;
};

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
    return std::pow(10, ((x_pos - bounds.getX()) / bounds.getWidth()) *
                        std::log10(x_lim.max / x_lim.min)) *
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
    return std::pow(10, ((bounds.getHeight() - (y_pos - bounds.getY())) /
                     bounds.getHeight()) *
                        std::log10(y_lim.max / y_lim.min)) *
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

static juce::Point<float> getDataPointFromGraphCoordinate(
    const juce::Point<float> pos,
    const CommonPlotParameterView common_plot_params) noexcept {
  const auto x = getXFromXCoordinate(
      pos.getX(), common_plot_params.graph_bounds.toFloat(),
      common_plot_params.x_lim, common_plot_params.x_scaling);
  const auto y = getYFromYCoordinate(
      pos.getY(), common_plot_params.graph_bounds.toFloat(),
      common_plot_params.y_lim, common_plot_params.y_scaling);

  return juce::Point<float>(x, y);
}

constexpr auto getXGraphValueLinear =
    [](const float x, const float x_scale, const float x_offset) -> auto{
  return (x * x_scale) - x_offset;
};

constexpr auto getYGraphValueLinear = [](const float y, const float y_scale,
                                         const float y_offset) -> float {
  return y_offset - (y * y_scale);
};

constexpr auto getXGraphPointsLogarithmic =
    [](const float x, const float x_scale_log, const float x_offset) -> float {
  return (x_scale_log * std::log10(x)) - x_offset;
};

constexpr auto getYGraphPointsLogarithmic =
    [](const float y, const float y_scale_log, const float y_offset) -> float {
  return y_offset - (y_scale_log * std::log10(y));
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
      x_scale = width / std::log10(x_lim.max / x_lim.min);
      x_offset = x_scale * std::log10(x_lim.min);
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
      y_scale = height / std::log10(y_lim.max / y_lim.min);
      y_offset = height + y_scale * std::log10(y_lim.min);
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

  const auto max_exp = lims.max != 0 ? std::log10(std::abs(lims.max)) : 0;
  const auto min_exp = lims.min != 0 ? std::log10(std::abs(lims.min)) : 0;

  const auto max_abs_exp = std::ceil(std::abs(max_exp));
  const auto min_abs_exp = std::ceil(std::abs(min_exp));

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
/*==========================From lnf.cpp======================================*/
/*============================================================================*/

static const std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator& custom_labels_it,
    const std::vector<std::string>::reverse_iterator& rend) {
  const auto retval = custom_labels_it++;

  if (retval != rend) {
    const auto val = *(retval);

    return val;
  }

  throw std::out_of_range("custom_labels_it is out of range.");
}

static std::vector<float> getLinearTicks(
    const std::size_t num_ticks, const cmp::Lim_f lim,
    const std::vector<float> previous_ticks) {
  std::vector<float> ticks(num_ticks);

  const auto diff = (lim.max - lim.min) / float(num_ticks);
  cmp::iota_delta(ticks.begin(), ticks.end(), lim.min + diff / 2.0f, diff);

  return ticks;
};

static std::vector<float> getLinearTicks_V2(
    const std::size_t num_ticks_per_power, const cmp::Lim_f lim,
    const std::vector<float>& previous_ticks) {
  if (!lim) return {};

  if (!previous_ticks.empty() && previous_ticks.front() < lim.min &&
      previous_ticks.back() > lim.max) {
    return previous_ticks;
  }

  const auto delta_min_max = lim.max - lim.min;
  const auto max_distance = delta_min_max / num_ticks_per_power;
  const auto log_max_distance = std::log10(max_distance);
  const auto log_max_distance_floor = std::floor(log_max_distance);
  const auto base_value = std::pow(10.f, log_max_distance_floor);
  auto multiplier = std::floor(max_distance / base_value);
  multiplier = multiplier == 0 ? 1 : multiplier;
  const auto delta = base_value * multiplier;
  std::vector<float> ticks;

  auto lim_min_round = lim.min;
  if (lim.min != 0) {
    const auto log_lim_min_floor = std::floor(std::log10(std::abs(lim.min)));
    const auto base_value_min = std::pow(10.f, log_lim_min_floor);
    auto multiplier_min = std::floor(lim.min / base_value_min);
    multiplier_min = multiplier_min == 0 ? 1 : multiplier_min;
    lim_min_round = base_value_min * multiplier_min;
  }

  if (lim_min_round < lim.min - delta) {
    const auto div = lim.min - delta - lim_min_round;
    const auto div_round = std::round(div / delta);
    lim_min_round += div_round * delta;
  }

  for (float i = lim_min_round - delta * num_ticks_per_power;
       i <= lim.max + delta * num_ticks_per_power; i += delta) {
    ticks.push_back(i);
  }

  return ticks;
};

static std::pair<float, float> getFirstAndEndFromPreviousTicks(
    const std::vector<float>& previous_ticks, const cmp::Lim_f lim) {
  auto start_value = 0.0f;
  {
    auto it = previous_ticks.begin();
    for (; it != previous_ticks.end(); ++it) {
      if (*it > lim.min) {
        if (it != previous_ticks.begin() && (it - 1) != previous_ticks.end()) {
          start_value = *(it - 1);
        } else {
          start_value = *it;
        }
        break;
      }
    }
  }

  auto end_value = 0.0f;
  {
    auto it = previous_ticks.rbegin();
    for (; it != previous_ticks.rend(); ++it) {
      if (*it < lim.max) {
        if (it != previous_ticks.rbegin() &&
            (it - 1) != previous_ticks.rend()) {
          end_value = *(it - 1);
        } else {
          end_value = *it;
        }
        break;
      }
    }
  }

  return {start_value, end_value};
}

static std::vector<float> getLogarithmicTicks(
    const std::size_t num_ticks_per_power, const cmp::Lim_f lim,
    const std::vector<float>& previous_ticks) {
  if (!lim) return {};

  const auto min_power = std::log10(lim.min);
  const auto max_power = std::log10(lim.max);

  const auto min_power_floor = std::floor(min_power);
  const auto max_power_ceil = std::ceil(max_power);

  std::vector<float> ticks;

  if (std::abs(max_power - min_power) < 1.0f && !previous_ticks.empty()) {
    ticks.resize(num_ticks_per_power);

    const auto [start_value, end_value] =
        getFirstAndEndFromPreviousTicks(previous_ticks, lim);

    auto delta = (end_value - start_value) / num_ticks_per_power;
    cmp::iota_delta(ticks.begin(), ticks.end(), lim.min, delta);

    return ticks;
  }

  for (float curr_power = min_power_floor; curr_power < max_power_ceil;
       ++curr_power) {
    const auto curr_pos_base = std::pow(10.f, curr_power);

    const auto delta =
        std::pow(10.f, curr_power + 1.f) / static_cast<float>(num_ticks_per_power);

    for (float i = 0; i < num_ticks_per_power; ++i) {
      const auto tick =
          floor((curr_pos_base + i * delta) / curr_pos_base) * curr_pos_base;

      ticks.push_back(tick);
    }
  }

  return ticks;
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