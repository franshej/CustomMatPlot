#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

struct node {
  void (*fun_ptr)(juce::Component *comp,
                  const std::string &test_name) = nullptr;
  std::string name;
  std::shared_ptr<node> next = nullptr;
};

extern std::shared_ptr<node> head;

#define ADD_PARENT_COMP(PARENT)            \
  std::shared_ptr<node> cur;               \
  for (cur = head; cur; cur = cur->next) { \
    cur->fun_ptr(PARENT, cur->name);       \
  }

static void add_test(void (*new_fun_ptr)(juce::Component *comp,
                                         const std::string &test_name),
                     const std::string &test_name) {
  static std::shared_ptr<node> curr{nullptr};

  if (!head) {
    head = std::make_shared<node>();
    head->fun_ptr = new_fun_ptr;
    curr = head;
    head->name = test_name;
  } else {
    curr->next = std::make_shared<node>();
    curr->next->fun_ptr = new_fun_ptr;
    curr->next->name = test_name;
    curr = curr->next;
  }
}

#define TEST(f)                                                     \
  static void f(juce::Component *parent_component,                  \
                const std::string &test_name);                      \
  struct f##_t_ {                                                   \
    f##_t_(void) { add_test(&f, #f); }                              \
  };                                                                \
  static std::unique_ptr<f##_t_> f##_ = std::make_unique<f##_t_>(); \
  static void f(juce::Component *parent_component, const std::string &test_name)

#define PARENT static_cast<MainComponent *>(parent_component)

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

class MainComponent : public juce::Component {
 public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::map<std::string, std::unique_ptr<scp::Plot>> *get_plot_holder() {
    return &m_plot_holder;
  }

  std::vector<std::shared_ptr<scp::PlotLookAndFeel>> lnf;
 private:
  std::map<std::string, std::unique_ptr<scp::Plot>> m_plot_holder;
  scp::Plot *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
