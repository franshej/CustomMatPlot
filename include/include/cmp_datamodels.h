#pragma once

#define IS_CXX_20 (_MSVC_LANG > 201703L || __cplusplus > 201703L)

#if __cpp_constexpr >= 201907L  // Check for a specific version of a feature
#define CONSTEXPR20 \
  constexpr  // This macro should be used in those cases where C++20 only allows
             // it. Example: virtual CONSTEXPR20 int foo() = 0;
#else
#define CONSTEXPR20
#endif

#if (__cpp_impl_three_way_comparison >= 201907L || IS_CXX_20)
#include <compare>
#endif

#ifdef __has_cpp_attribute
#if (__has_cpp_attribute(unlikely) || IS_CXX_20)
#define UNLIKELY [[unlikely]]
#else
#define UNLIKELY
#endif
#endif

#include <optional>

#include "juce_gui_basics/juce_gui_basics.h"

namespace cmp {

/*============================================================================*/

/**< Enum to define the scaling of an axis. */
enum class Scaling : uint32_t {
  linear,     /**< Linear scaling of the graph line. */
  logarithmic /**< Logarithmic scaling of the graph line. */
};

/*============================================================================*/

struct Marker {
  /** Type of marker. */
  enum class Type {
    Circle,
    Pentagram,
    Square,
    UpTriangle,
    RightTriangle,
    DownTriangle,
    LeftTriangle
  } type;

  /** Contructor marker type only. */
  Marker(const Marker::Type t) : type{t} {};

  /**  Marker outline color. */
  std::optional<juce::Colour> EdgeColour;

  /** Marker interior color. */
  std::optional<juce::Colour> FaceColour;

  /** PathStrokeType used when drawing the edge line of the marker. */
  juce::PathStrokeType edge_stroke_type{
      1.0f, juce::PathStrokeType::JointStyle::mitered,
      juce::PathStrokeType::EndCapStyle::rounded};

  static juce::Path getMarkerPathFrom(Marker marker, const float length) {
    juce::Path path;

    auto addUpTriangleTo = [length](auto &path) {
      path.addTriangle({0.0f, -length / 2.0f}, {-length / 2.0f, length / 2.0f},
                       {length / 2.0f, length / 2.0f});
    };

    switch (marker.type) {
      case Marker::Type::Circle:
        path.addEllipse({-length / 2.0f, -length / 2.0f, length, length});
        break;
      case Marker::Type::Pentagram:
        path.addStar({0, 0}, 5, length / 4.0f, length / 2.0f);
        break;
      case Marker::Type::Square:
        path.addRectangle(-length / 2.0f, -length / 2.0f, length, length);
        break;
      case Marker::Type::UpTriangle:
        addUpTriangleTo(path);
        break;
      case Marker::Type::RightTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi / 2.0f, 0.0f, 0.0f));
        break;
      case Marker::Type::DownTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi, 0.0f, 0.0f));
        break;
      case Marker::Type::LeftTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi * 3.0f / 2.0f, 0.0f, 0.0f));
        break;
      default:
        break;
    }

    return std::move(path);
  }
};

/** Attributes of a single graph. */
struct GraphAttribute {
  /** Colour of the graph_line. */
  std::optional<juce::Colour> graph_colour;

  /** Custom path stroke @see juce::PathStrokeType */
  std::optional<juce::PathStrokeType> path_stroke_type;

  /** Use dash_lengths to draw dashed graph_line. E.g. dashed_lengths = {2,
   * 2, 4, 6} will draw a line of 2 pixels, skip 2 pixels, draw 3 pixels, skip
   * 6 pixels, and then repeat. */
  std::optional<std::vector<float>> dashed_lengths;

  /** Set the opacity of the graph_line. Value must be between 0 (transparent)
   * and 1.0 (opaque). */
  std::optional<float> graph_line_opacity;

  /** The type of marker drawn on each graph point. */
  std::optional<cmp::Marker> marker;

  /** Callback function which is triggerd for every plotted graph_point. E.g.
   * Can be used to do custom plot markers for each graph_point.*/
  std::function<void(juce::Graphics& g, juce::Point<float> data_point,
                     juce::Point<float> graph_point)>
      on_graph_point_paint{nullptr};
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
typedef std::vector<GraphAttribute> GraphAttributeList;

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

#if IS_CXX_20
  auto operator<=>(const Lim<ValueType>&) const noexcept = default;
#else
  /** No spaceship  :( */
  bool operator==(const Lim<ValueType>& rhs) const noexcept {
    return min == rhs.min && max == rhs.max;
  }
#endif

  constexpr explicit operator bool() const noexcept {
    constexpr auto zero = static_cast<ValueType>(0);

    return max != zero || min != zero;
  }

  constexpr bool isMinOrMaxZero() const noexcept {
    constexpr auto zero = static_cast<ValueType>(0);

    return max == zero || min == zero;
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

/** @brief A view of some common plot parameters. */
struct CommonPlotParameterView {
  CommonPlotParameterView(const juce::Rectangle<int>& _graph_bounds,
                          const Lim_f& _x_lim, const Lim_f& _y_lim)
      : graph_bounds{_graph_bounds}, x_lim{_x_lim}, y_lim{_y_lim} {};
  CommonPlotParameterView(const juce::Rectangle<int>&&, const Lim_f&&,
                          const Lim_f&&) = delete;  // prevents rvalue binding
  const juce::Rectangle<int>& graph_bounds;
  const Lim_f &x_lim, &y_lim;
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
      return {2, 2};
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

  auto num_digits_before_sign = 0;
  if (lims_diff_log >= 0 && largest_exp >= 0) {
    if (int(std::ceil(std::log10(value))) == int(largest_abs_exp)) {
      num_digits_before_sign += largest_abs_exp + 1;
    } else {
      num_digits_before_sign += largest_abs_exp;
    }
  } else if (lims_diff_log < 0 && largest_exp > 0) {
    num_digits_before_sign += largest_abs_exp + num_digits_diff;
  } else if (lims_diff_log < 0 && largest_exp < 0) {
    num_digits_before_sign += std::max(num_digits_diff, largest_abs_exp);
  }

  const auto num_digits_before_exponent_sign =
      std::ceil(num_digits_before_sign) + int(value < 0);

  const auto num_digits_before_checking_ending_character =
      num_digits_before_exponent_sign + 2 * int(largest_exp < 0);

  auto value_text_out =
      value_text.substr(0u, num_digits_before_checking_ending_character);

  if (value_text_out.back() == '.') {
    value_text_out =
        value_text.substr(0u, num_digits_before_checking_ending_character + 1);
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

}  // namespace cmp
