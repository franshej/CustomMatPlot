#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

struct node {
  void (*fun_ptr)(juce::Component *comp,
                  const std::string &test_name) = nullptr;
  std::string name;
  std::shared_ptr<node> next = nullptr;
  std::shared_ptr<node> tail = nullptr;
};

extern std::shared_ptr<node> head;

#define ADD_PARENT_COMP(COMP)              \
  std::shared_ptr<node> cur;               \
  for (cur = head; cur; cur = cur->next) { \
    cur->fun_ptr(COMP, cur->name);         \
  }

static void g_test_add(void (*new_fun_ptr)(juce::Component *comp,
                                           const std::string &test_name),
                       const std::string &test_name) {
  if (head == NULL) {
    head = std::make_shared<node>();
    head->fun_ptr = new_fun_ptr;
    head->tail = head;
    head->name = test_name;
  } else {
    head->tail->next = std::make_shared<node>();
    head->tail->next->fun_ptr = new_fun_ptr;
    head->tail->next->name = test_name;
    head->tail = head->tail->next;
  }
}

#define TEST(f)                                                       \
  static void f(juce::Component *thiz, const std::string &test_name); \
  struct f##_t_ {                                                     \
    f##_t_(void) { g_test_add(&f, #f); }                              \
  };                                                                  \
  static std::unique_ptr<f##_t_> f##_ = std::make_unique<f##_t_>();   \
  static void f(juce::Component *thiz, const std::string &test_name)

#define THIS static_cast<MainComponent *>(thiz)

#define ADD_PLOT(TYPE)                                                \
  (*(THIS->get_plot_holder()))[test_name] = std::make_unique<TYPE>(); \
  thiz->addAndMakeVisible(                                            \
      THIS->get_plot_holder()->find(test_name)->second.get());

#define GET_PLOT THIS->get_plot_holder()->find(test_name)->second.get()

#define PLOT_Y(Y)             \
  {                           \
    ADD_PLOT(scp::LinearPlot) \
    GET_PLOT->Plot(Y);        \
  }

#define SEMI_PLOT_Y(Y)       \
  {                          \
    ADD_PLOT(scp::SemiPlotX) \
    GET_PLOT->Plot(Y);       \
  }

#define PLOT_XY(X, Y)         \
  {                           \
    ADD_PLOT(scp::LinearPlot) \
    GET_PLOT->Plot(Y, X);     \
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

class MainComponent : public juce::Component {
 public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::map<std::string, std::unique_ptr<scp::PlotBase>> *get_plot_holder() {
    return &m_plot_holder;
  }

 private:
  std::map<std::string, std::unique_ptr<scp::PlotBase>> m_plot_holder;
  scp::PlotBase *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
