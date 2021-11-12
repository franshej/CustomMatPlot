#pragma once

namespace scp {

/*============================================================================*/

/**< Enum to define the type of graph line. */
enum GraphType : uint32_t {
  graph_line, /**< Graph line. */
  grid_line   /**< GridLine used for the grids.*/
};

/**< Enum to define the scaling of an axis. */
enum Scaling : uint32_t {
  linear,     /**< Linear scaling of the graph line. */
  logarithmic /**< Logarithmic scaling of the graph line. */
};

/*============================================================================*/

class BaseGrid;
class Frame;
class GraphLine;
class PlotLookAndFeel;
class PlotLabel;
class Legend;
class Zoom;

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

typedef std::vector<std::unique_ptr<GraphLine>> GridLines;
typedef std::vector<std::unique_ptr<GraphLine>> GraphLines;
typedef std::vector<juce::Point<float>> GraphPoints;
typedef std::vector<std::pair<std::string, juce::Rectangle<int>>> LabelVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<juce::Colour> ColourVector;

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
  return x_offset + (x * x_scale);
};

constexpr auto getXGraphPointsLogarithmic =
    [](const float x, const float x_scale_log, const float x_offset) -> float {
  return (x_scale_log * log10(x)) - x_offset;
};

constexpr auto getXScaleAndOffset =
    [](const float width, const Lim_f& x_lim,
       Scaling scaling) -> std::tuple<float, float> {
  float x_scale, x_offset;

  switch (scaling) {
    case scp::linear:
      x_scale = width / (x_lim.max - x_lim.min);
      x_offset = -(x_lim.min * x_scale);
      break;
    case scp::logarithmic:
      x_scale = width / log10(x_lim.max / x_lim.min);
      x_offset = log10(x_lim.min);
      break;
    default:
      break;
  }

  return {x_scale, x_offset};
};

constexpr auto getXGraphPointConversionFunction =
    [&](const Scaling scaling) -> std::function<float(float, float, float)> {
  switch (scaling) {
    case Scaling::linear:
      return getXGraphPointsLinear;
      break;
    case Scaling::logarithmic:
      return getXGraphPointsLogarithmic;
      break;
    default:
      return getXGraphPointsLinear;
      break;
  }
};

/*============================================================================*/

}  // namespace scp
