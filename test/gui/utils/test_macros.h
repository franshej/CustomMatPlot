#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "spl_plot.h"
#include "test_utils.h"

#define TEST(f, category)                                           \
  static void f(juce::Component *parent_component,                  \
                const std::string &test_name);                      \
  struct f##_t_ {                                                   \
    f##_t_(void) { add_test(&f, #f, #category); }                   \
  };                                                                \
  static std::unique_ptr<f##_t_> f##_ = std::make_unique<f##_t_>(); \
  static void f(juce::Component *parent_component, const std::string &test_name)

#define PARENT static_cast<TestHandler *>(parent_component)

#define ADD_PLOT_TYPE(TYPE)                               \
  auto __plot_comp = std::make_unique<TYPE>();            \
  parent_component->addAndMakeVisible(__plot_comp.get()); \
  (*(PARENT->getPlotHolder()))[test_name].plot = std::move(__plot_comp);

#define ADD_TIMER(DT_MS)                          \
  (*(PARENT->getPlotHolder()))[test_name].timer = \
      std::make_unique<TimerCallback>(DT_MS);

#define GET_PLOT PARENT->getPlotHolder()->find(test_name)->second.plot.get()

#define GET_TIMER_CB \
  PARENT->getPlotHolder()->find(test_name)->second.timer.get()->onTimerCallback

#define ADD_PLOT ADD_PLOT_TYPE(scp::Plot)

#define ADD_SEMI_LOG_X ADD_PLOT_TYPE(scp::SemiLogX)

#define ADD_SEMI_LOG_Y ADD_PLOT_TYPE(scp::SemiLogY)

#define PLOT_Y(Y) \
  { GET_PLOT->plot(Y); }

#define PLOT_XY(Y, X) \
  { GET_PLOT->plot(Y, X); }

#define PLOT_XY_ATTRI(Y, X, ATTRI) \
  { GET_PLOT->plot(Y, X, ATTRI); }

#define REALTIMEPLOT(Y) \
  { GET_PLOT->realTimePlot(Y); }

#define X_LIM(MIN, MAX) GET_PLOT->xLim(MIN, MAX);

#define Y_LIM(MIN, MAX) GET_PLOT->yLim(MIN, MAX);

#define GRID_ON GET_PLOT->gridON(true, false);

#define TINY_GRID_ON GET_PLOT->gridON(true, true);

#define MAKE_GRAPH_DASHED(D_LENGTHS, GRAPH_INDEX) \
  GET_PLOT->makeGraphDashed(D_LENGTHS, GRAPH_INDEX);

#define X_LABEL(TEXT) GET_PLOT->setXLabel(TEXT);

#define Y_LABEL(TEXT) GET_PLOT->setYLabel(TEXT);

#define TITLE(TEXT) GET_PLOT->setTitle(TEXT);

#define X_LABELS(LABELS) GET_PLOT->setXTickLabels(LABELS);

#define Y_LABELS(LABELS) GET_PLOT->setYTickLabels(LABELS);

#define X_TICKS(TICKS) GET_PLOT->setXTicks(TICKS);

#define Y_TICKS(TICKS) GET_PLOT->setYTicks(TICKS);

#define LEGEND(TEXTS) GET_PLOT->setLegend(TEXTS);
