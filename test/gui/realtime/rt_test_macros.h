#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

#include "rt_test_handler.h"

#define TEST(f)                                                     \
  static void f(juce::Component *parent_component,                  \
                const std::string &test_name);                      \
  struct f##_t_ {                                                   \
    f##_t_(void) { add_test(&f, #f); }                              \
  };                                                                \
  static std::unique_ptr<f##_t_> f##_ = std::make_unique<f##_t_>(); \
  static void f(juce::Component *parent_component, const std::string &test_name)

#define PARENT static_cast<RTTestHandler *>(parent_component)

#define ADD_PLOT(TYPE)                                    \
  auto __plot_comp = std::make_unique<TYPE>();            \
  parent_component->addAndMakeVisible(__plot_comp.get()); \
  (*(PARENT->get_plot_holder()))[test_name] = std::move(__plot_comp);

#define GET_PLOT PARENT->get_plot_holder()->find(test_name)->second.get()

#define PLOT_Y(Y)       \
  {                     \
    ADD_PLOT(scp::Plot) \
    GET_PLOT->plot(Y);  \
  }

#define SEMI_LOG_X(X)       \
  {                         \
    ADD_PLOT(scp::SemiLogX) \
    GET_PLOT->plot(X);      \
  }

#define SEMI_LOG_XY(X, Y)   \
  {                         \
    ADD_PLOT(scp::SemiLogX) \
    GET_PLOT->plot(X, Y);   \
  }

#define SEMI_LOG_Y(Y)       \
  {                         \
    ADD_PLOT(scp::SemiLogY) \
    GET_PLOT->plot(Y);      \
  }

#define PLOT_XY(X, Y)     \
  {                       \
    ADD_PLOT(scp::Plot)   \
    GET_PLOT->plot(Y, X); \
  }

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
