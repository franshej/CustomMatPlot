#pragma once

namespace scp {
/*============================================================================*/

class PlotLookAndFeel;
class PlotLabel;
class BaseGrid;
class GraphLine;

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

/*============================================================================*/

#define jassert_return(expression, help_text) \
  jassert(expression);                        \
  if (!expression) return;

/*============================================================================*/

}  // namespace scp
