#pragma once

namespace scp {
/*============================================================================*/

class BaseGrid;
class Frame;
class GraphLine;
class PlotLookAndFeel;
class PlotLabel;

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
typedef std::vector<juce::Point<float>> GraphPoints;
typedef std::vector<std::pair<std::string, juce::Rectangle<int>>> LabelVector;
typedef std::vector<std::string> StringVector;

/*============================================================================*/

#define jassert_return(expression, help_text) \
  jassert(expression);                        \
  if (!expression) return;

/*============================================================================*/

template <class value_type>
constexpr std::tuple<value_type, value_type, value_type, value_type>
getRectangleMeasures(juce::Rectangle<int> grid_area) {
  const auto x = static_cast<value_type>(grid_area.getX());
  const auto y = static_cast<value_type>(grid_area.getY());
  const auto width = static_cast<value_type>(grid_area.getWidth());
  const auto height = static_cast<value_type>(grid_area.getHeight());
  return std::make_tuple(x, y, width, height);
}

/*============================================================================*/

}  // namespace scp
