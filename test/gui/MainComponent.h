#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

struct node {
  void (*fun_ptr)(juce::Component *comp,
                  const std::string &test_name) = nullptr;
  std::string name;
  node *next = NULL;
  node *tail = NULL;
};

extern node *head;

#define ADD_PARENT_COMP(COMP)              \
  struct node *cur;                        \
  for (cur = head; cur; cur = cur->next) { \
    cur->fun_ptr(COMP, cur->name);         \
  }

static void g_test_add(void (*new_fun_ptr)(juce::Component *comp,
                                           const std::string &test_name),
                       const std::string &test_name) {
  if (head == NULL) {
    head = new node;
    head->fun_ptr = new_fun_ptr;
    head->tail = head;
    head->name = test_name;
  } else {
    head->tail->next = new node;
    head->tail->next->fun_ptr = new_fun_ptr;
    head->tail->next->name = test_name;
    head->tail = head->tail->next;
  }
}

#ifdef __cplusplus
#define TEST(f)                                                       \
  static void f(juce::Component *thiz, const std::string &test_name); \
  struct f##_t_ {                                                     \
    f##_t_(void) { g_test_add(&f, #f); }                              \
  };                                                                  \
  static f##_t_ f##_;                                                 \
  static void f(juce::Component *thiz, const std::string &test_name)
#else
#define TEST(f)                                                                  \
  static void f(juce::Component *thiz);                                          \
  static void __attribute__((constructor(101)) __construct_##f(void) {        \
    g_test_add(&f);                                                            \
  }                                                                            \
  static void f(juce::Component *thiz)
#endif

#define THIS static_cast<MainComponent *>(thiz)

#define ADD_PLOT(TYPE)                                                     \
(*(THIS->get_plot_holder()))[test_name] = std::make_unique<TYPE>(); \
  thiz->addAndMakeVisible(((MainComponent *)thiz)                          \
                              ->get_plot_holder()                          \
                              ->find(test_name)                            \
                              ->second.get());

#define GET_PLOT \
  ((MainComponent *)thiz)->get_plot_holder()->find(test_name)->second.get()

#define PLOT_Y(Y)             \
  {                           \
    ADD_PLOT(LinearPlot)      \
    GET_PLOT->updateYData(Y); \
  }

#define SEMI_PLOT_Y(Y)        \
  {                           \
    ADD_PLOT(SemiPlotX)      \
    GET_PLOT->updateYData(Y); \
  }

#define PLOT_XY(X, Y)         \
  {                           \
    ADD_PLOT(LinearPlot)      \
    GET_PLOT->updateYData(Y); \
    GET_PLOT->updateXData(X); \
  }

#define X_LIM(MIN, MAX) GET_PLOT->xLim(MIN, MAX);

#define Y_LIM(MIN, MAX) GET_PLOT->yLim(MIN, MAX);

#define GRID_ON GET_PLOT->gridON(true);

#define MAKE_GRAPH_DASHED(D_LENGTHS, GRAPH_INDEX) \
  GET_PLOT->makeGraphDashed(D_LENGTHS, GRAPH_INDEX);

#define X_LABEL(TEXT) GET_PLOT->setXLabel(TEXT);

#define Y_LABEL(TEXT) GET_PLOT->setYLabel(TEXT);

#define TITLE(TEXT) GET_PLOT->setTitle(TEXT);

class MainComponent : public juce::Component {
 public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::map<std::string, std::unique_ptr<Plot>> *get_plot_holder() {
    return &m_plot_holder;
  }

 private:
  std::map<std::string, std::unique_ptr<Plot>> m_plot_holder;
  Plot *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
