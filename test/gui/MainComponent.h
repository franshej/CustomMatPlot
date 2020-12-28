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

#define ADD_PARENT_COMP(COMP)                                                  \
  struct node *cur;                                                            \
  for (cur = head; cur; cur = cur->next) {                                     \
    cur->fun_ptr(COMP, cur->name);                                             \
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
#define TEST(f)                                                                \
  static void f(juce::Component *thiz, const std::string &test_name);          \
  struct f##_t_ {                                                              \
    f##_t_(void) { g_test_add(&f, #f); }                                       \
  };                                                                           \
  static f##_t_ f##_;                                                          \
  static void f(juce::Component *thiz, const std::string &test_name)
#else
#define TEST(f)                                                                  \
  static void f(juce::Component *thiz);                                          \
  static void __attribute__((constructor(101)) __construct_##f(void) {        \
    g_test_add(&f);                                                            \
  }                                                                            \
  static void f(juce::Component *thiz)
#endif

#define PLOT_Y(Y)                                                              \
  {                                                                            \
    ((MainComponent *)thiz)                                                    \
        ->get_plot_holder()                                                    \
        ->push_back({std::make_unique<LinearPlot>(), test_name});              \
    ((MainComponent *)thiz)->get_plot_holder()->back().first->updateYData(Y);  \
    thiz->addAndMakeVisible(                                                   \
        ((MainComponent *)thiz)->get_plot_holder()->back().first.get());       \
  }

#define PLOT_XY(X, Y)                                                          \
  {                                                                            \
    ((MainComponent *)thiz)                                                    \
        ->get_plot_holder()                                                    \
        ->push_back({std::make_unique<LinearPlot>(), test_name});              \
    ((MainComponent *)thiz)->get_plot_holder()->back().first->updateYData(Y);  \
    ((MainComponent *)thiz)->get_plot_holder()->back().first->updateXData(X);  \
    thiz->addAndMakeVisible(                                                   \
        ((MainComponent *)thiz)->get_plot_holder()->back().first.get());       \
  }

#define X_LIM(MIN, MAX)                                                        \
  ((MainComponent *)thiz)->get_plot_holder()->back().first->xLim(MIN, MAX);

#define Y_LIM(MIN, MAX)                                                        \
  ((MainComponent *)thiz)->get_plot_holder()->back().first->yLim(MIN, MAX);

class MainComponent : public juce::Component {
public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::vector<std::pair<std::unique_ptr<Plot>, std::string>> *
  get_plot_holder() {
    return &m_plot_holder;
  }

private:
  std::vector<std::pair<std::unique_ptr<Plot>, std::string>> m_plot_holder;
  Plot *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
