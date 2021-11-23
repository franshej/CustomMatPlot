#pragma once

#if __cplusplus >= 202002L
#define CONSTEXPR20 \
  constexpr  // This macro should be used in those cases where C++20 only allows
             // it. Example: virtual CONSTEXPR20 foo() = 0;
#else
#define CONSTEXPR20
#endif

namespace scp {
/*============================================================================*/

class BaseGrid;
class Frame;
class GraphLine;
class PlotLookAndFeel;
class PlotLabel;
class Legend;
class Zoom;

/*============================================================================*/

typedef std::vector<std::unique_ptr<GraphLine>> GridLines;
typedef std::vector<std::unique_ptr<GraphLine>> GraphLines;
typedef std::vector<juce::Point<float>> GraphPoints;
typedef std::vector<std::pair<std::string, juce::Rectangle<int>>> LabelVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<juce::Colour> ColourVector;

/*============================================================================*/

/**< Enum to define the type of graph line. */
enum class GraphType : uint32_t {
  graph_line, /**< Graph line. */
  grid_line   /**< GridLine used for the grids.*/
};

/**< Enum to define the scaling of an axis. */
enum class Scaling : uint32_t {
  linear,     /**< Linear scaling of the graph line. */
  logarithmic /**< Logarithmic scaling of the graph line. */
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

/*============================================================================*/

template <class T>
struct Lim {
  T min;
  T max;
};

typedef Lim<float> Lim_f;

/*============================================================================*/

class LookAndFeelMethodsBase {};

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

constexpr float getXFromXCoordinate(const float x_pos, const float graph_x,
                                    const float width, const Lim_f x_lim,
                                    const Scaling x_scaling) noexcept {
  const auto coordinateToXLinear = [&]() {
    const auto x_scale = width / (x_lim.max - x_lim.min);
    return ((x_pos - graph_x) / x_scale) + x_lim.min;
  };

  const auto coordinateToXLog = [&]() {
    return powf(10,
                ((x_pos - graph_x) / width) * log10(x_lim.max / x_lim.min)) *
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

constexpr float getYFromYCoordinate(const float y_pos, const float graph_y,
                                    const float height, const Lim_f y_lim,
                                    const Scaling y_scaling) noexcept {
  const auto coordinateToYLinear = [&]() {
    const auto y_scale = height / std::abs(y_lim.max - y_lim.min);

    return y_lim.max - ((y_pos - graph_y) / y_scale);
  };
  switch (y_scaling) {
    case Scaling::linear:
      return coordinateToYLinear();
      break;
    case Scaling::logarithmic:
      jassert(false, "Log scale for y axis is not implemented.");
      return -1;
      break;
    default:
      return coordinateToYLinear();
      break;
  }
}

constexpr auto getXGraphPointsLinear = [](const float x, const float x_scale,
                                          const float x_offset) -> float {
  return (x * x_scale) - x_offset;
};

constexpr auto getYGraphPointsLinear = [](const float y, const float y_scale,
                                          const float y_offset) -> float {
  return y_offset - (y * y_scale);
};

constexpr auto getXGraphPointsLogarithmic =
    [](const float x, const float x_scale_log, const float x_offset) -> float {
  return (x_scale_log * log10(x)) - x_offset;
};

constexpr auto getYGraphPointsLogarithmic =
    [](const float y, const float y_scale_log, const float y_offset) -> float {
  // not implemented.
  jassertfalse;
  return y;
};

constexpr auto getXScaleAndOffset =
    [](const float width, const Lim_f& x_lim,
       Scaling scaling) -> std::tuple<float, float> {
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
       Scaling scaling) -> std::tuple<float, float> {
  float y_scale, y_offset;

  switch (scaling) {
    case Scaling::linear:
      y_scale = height / (y_lim.max - y_lim.min);
      y_offset = height + (y_lim.min * y_scale);
      break;
    case Scaling::logarithmic:
      break;
    default:
      break;
  }

  return {y_scale, y_offset};
};

/*============================================================================*/

}  // namespace scp
