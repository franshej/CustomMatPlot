#include "MainComponent.h"
#define PI2 6.28318530718

#define PLOT_Y(Y)                                                              \
  {                                                                            \
    ((MainComponent *)thiz)                                                    \
        ->get_plot_holder()                                                    \
        ->push_back(std::make_unique<LinearPlot>());                           \
    ((MainComponent *)thiz)->get_plot_holder()->back()->updateYData(Y);        \
    thiz->addAndMakeVisible(                                                   \
        ((MainComponent *)thiz)->get_plot_holder()->back().get());             \
  }

#define PLOT_XY(X, Y)                                                          \
  {                                                                            \
    ((MainComponent *)thiz)                                                    \
        ->get_plot_holder()                                                    \
        ->push_back(std::make_unique<LinearPlot>());                           \
    ((MainComponent *)thiz)->get_plot_holder()->back()->updateYData(Y);        \
    ((MainComponent *)thiz)->get_plot_holder()->back()->updateXData(X);        \
    thiz->addAndMakeVisible(                                                   \
        ((MainComponent *)thiz)->get_plot_holder()->back().get());             \
  }

#define X_LIM(MIN, MAX)                                                        \
  ((MainComponent *)thiz)->get_plot_holder()->back()->xLim(MIN, MAX);

#define Y_LIM(MIN, MAX)                                                        \
  ((MainComponent *)thiz)->get_plot_holder()->back()->yLim(MIN, MAX);

struct node {
  void (*fun_ptr)(juce::Component *comp) = nullptr;
  node *next = NULL;
  node *tail = NULL;
} *head = NULL;

static void g_test_add(void (*new_fun_ptr)(juce::Component *comp)) {
  if (head == NULL) {
    head = new node;
    head->fun_ptr = new_fun_ptr;
    head->tail = head;
  } else {
    head->tail->next = new node;
    head->tail->next->fun_ptr = new_fun_ptr;
    head->tail = head->tail->next;
  }
}

#ifdef __cplusplus
#define TEST(f)                                                                \
  static void f(juce::Component *thiz);                                        \
  struct f##_t_ {                                                              \
    f##_t_(void) { g_test_add(&f); }                                           \
  };                                                                           \
  static f##_t_ f##_;                                                          \
  static void f(juce::Component *thiz)
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
    cur->fun_ptr(COMP);                                                        \
  }

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

MainComponent::MainComponent() {
  setSize(600, 400);
  ADD_PARENT_COMP(this);

  const auto side_len = std::sqrt(m_plot_holder.size());
  const auto is_not_rational = (side_len - floor(side_len)) < 10e-4;

  auto num_cols = 0u;
  auto num_rows = 0u;

  if (is_not_rational) {
    num_cols = static_cast<size_t>(side_len);
    num_rows = static_cast<size_t>(side_len);
  } else {
    num_cols = static_cast<size_t>(floor(side_len + 1));
    num_rows = static_cast<size_t>(floor(side_len + 1));
  }

  auto height = getHeight() / (num_rows);
  auto width = getWidth() / (num_cols);

  auto i = 0u;
  for (int col = 0; col < num_cols; ++col) {
    for (int row = 0; row < num_rows; ++row) {
      if (i < m_plot_holder.size()) {
        m_plot_holder[i]->setBounds(col * width, row * height, width, height);
        i++;
      }
    }
  }
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

void MainComponent::resized() {}
