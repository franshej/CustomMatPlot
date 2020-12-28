#include "MainComponent.h"
#define PI2 6.28318530718

extern node *head = NULL;

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

void MainComponent::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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
