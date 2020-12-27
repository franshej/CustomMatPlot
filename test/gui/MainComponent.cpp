#include "MainComponent.h"
#define PI2 6.28318530718

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

struct node {
  void (*fun_ptr)(juce::Component *comp,
                  const std::string &test_name) = nullptr;
  std::string name;
  node *next = NULL;
  node *tail = NULL;
} *head = NULL;

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

#define ADD_PARENT_COMP(COMP)                                                  \
  struct node *cur;                                                            \
  for (cur = head; cur; cur = cur->next) {                                     \
    cur->fun_ptr(COMP, cur->name);                                             \
  }

TEST(test_flat_curve_1000) {
  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  PLOT_Y({y_test_data});
};

TEST(test_sinus_auto_lim) {
  std::vector<float> test_data = std::vector<float>(100);
  std::iota(test_data.begin(), test_data.end(), 0.f);
  for (auto &y : test_data) {
    y = std::sin(y * PI2 / test_data.size());
  }
  PLOT_Y({test_data});
}

TEST(test_flat_curve) {
  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0);
  PLOT_Y({y_test_data});
};

TEST(test_two_sine) {
  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(2, std::vector<float>(100));
  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = std::sin(y * PI2 / y_vec.size()) + i;
    }
    i++;
  }

  std::vector<std::vector<float>> test_data_x =
      std::vector<std::vector<float>>(2, std::vector<float>(100));
  std::iota(test_data_x.front().begin(), test_data_x.front().end(), 0);
  std::iota(test_data_x.back().begin(), test_data_x.back().end(), -50);

  PLOT_XY(test_data_x, test_data_y);
}

TEST(test_x_lim) {
  std::vector<float> test_data_y = std::vector<float>(100);
  std::iota(test_data_y.begin(), test_data_y.end(), 0.f);
  for (auto &y : test_data_y) {
    y = std::sin(y * PI2 / test_data_y.size());
  }

  std::vector<float> test_data_x = std::vector<float>(100);
  std::iota(test_data_x.begin(), test_data_x.end(), -50);

  PLOT_XY({test_data_x}, {test_data_y});
  X_LIM(0, 50);
}

TEST(test_y_lim) {
  std::vector<float> test_data_y = std::vector<float>(100);
  std::iota(test_data_y.begin(), test_data_y.end(), 0.f);
  for (auto &y : test_data_y) {
    y = std::sin(y * PI2 / test_data_y.size());
  }

  PLOT_Y({test_data_y});
  Y_LIM(0, 1);
}

static juce::Rectangle<int> getScreenArea() {
  return juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
}

MainComponent::MainComponent() : m_menu_label("", "Tests: ") {
  setSize(1200, 800);
  ADD_PARENT_COMP(this);

  addAndMakeVisible(m_test_menu);
  addAndMakeVisible(m_menu_label);

  for (auto i = 0u; i < m_plot_holder.size(); ++i) {
    m_test_menu.addItem(m_plot_holder[i].second, i + 1);
  }

  for (auto &plot : m_plot_holder) {
    plot.first->setBounds(0, getScreenArea().getHeight() / 15, getWidth(),
                          getHeight() - getScreenArea().getHeight() / 15);
	plot.first->setVisible(false);
  }

  m_test_menu.onChange = [this]() {
	  if (m_current_plot != nullptr) {
		  m_current_plot->setVisible(false);
	  }
	  const auto id = m_test_menu.getSelectedId();
	  if (!m_plot_holder.empty()) {
		  m_current_plot = m_plot_holder[id - 1].first.get();
		  m_current_plot->setVisible(true);
	  }
  };
}

/*


void test_y_lim(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(),
test_data.back().y_values.end(), 0); for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size());
    }
    LinearPlot plot(400, 200, 200, 100);
    plot.yLim(-1, 0);
    plot.plot(g, test_data);
}
*/
void MainComponent::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  /*

std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(),
0); test_data.back().x_values = std::vector<float>(10);
std::iota(test_data.back().x_values.begin() + 5,
test_data.back().x_values.end(), 0); test_data.back().x_values[0] = -3;
test_plot3.plot(test_data);

test_data.back().x_values = std::vector<float>(10);
for (auto i = 0u; i < test_data.back().x_values.size(); ++i)
{
test_data.back().x_values[i] = i * 20;
}
test_data.back().x_values[0] = 3;
test_plot4.plot(test_data);

test_simple_ramp(g);
test_sinus_auto_lim(g);
test_two_sinus_auto_lim(g);
test_x_lim(g);
test_y_lim(g);
*/
}

void MainComponent::resized() {

  m_test_menu.setBounds(0, getScreenArea().getHeight() / 30, getWidth() / 2,
                        getScreenArea().getHeight() / 30);
  m_menu_label.setBounds(0, 0, getWidth() / 2,
                         getScreenArea().getHeight() / 30);

  for (auto &plot : m_plot_holder) {
    plot.first->setBounds(0, getScreenArea().getHeight() / 15, getWidth(),
                          getHeight() - getScreenArea().getHeight() / 15);
  }
}
