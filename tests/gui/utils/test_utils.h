/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cmp_plot.h>

template <class ContainerType>
static ContainerType& getPlotFromID(
    std::map<std::string, ContainerType>& plot_holder, const int id) {
  if (id > plot_holder.size()) {
    throw std::invalid_argument("Index out of range.");
  }

  auto it = plot_holder.begin();

  for (int i = 1; i < plot_holder.size() + 1; ++i) {
    if (id == i) break;
    it++;
  }

  return it->second;
}

static juce::Rectangle<int> getScreenArea() {
  return juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
}

struct TimerCallback : public juce::Timer {
  std::function<void(const int dt_ms)> onTimerCallback = nullptr;

  TimerCallback(const int dt_ms) { startTimer(dt_ms); }

  void timerCallback() override {
    if (onTimerCallback) {
      onTimerCallback(this->getTimerInterval());
    }
  }
};

struct TestItem {
  std::function<void(juce::Component*, const std::string&)> test_function{
      nullptr};
  std::string test_name{""};
  int dt_ms{0};
};

struct node {
  TestItem test_item;
  std::shared_ptr<node> next = nullptr;
};

struct PlotAndTimer {
  std::unique_ptr<cmp::Plot> plot;
  std::unique_ptr<TimerCallback> timer;
};

class TestHandler : public juce::Component {
 public:
  TestHandler(const std::string category)
      : m_menu_label("", "Tests: "), m_category(category) {
    setSize(900, 600);

    // Calls all the test functions created by the user.
    std::shared_ptr<node> cur;
    for (cur = TestHandler::head[m_category]; cur; cur = cur->next) {
      cur->test_item.test_function(this, cur->test_item.test_name);
    }

    addAndMakeVisible(m_test_menu);
    addAndMakeVisible(m_menu_label);

    auto it = m_plot_holder.begin();
    for (auto i = 0u; i < m_plot_holder.size(); ++i) {
      m_test_menu.addItem((*it++).first, i + 1);
    }

    for (auto& plot : m_plot_holder) {
      plot.second.plot->setBounds(
          0, getScreenArea().getHeight() / 15, getWidth(),
          getHeight() - getScreenArea().getHeight() / 15);
      plot.second.plot->setVisible(false);
    }

    m_test_menu.onChange = [this]() {
      if (m_current_plot != nullptr) {
        m_current_plot->plot->setVisible(false);
      }
      const auto id = m_test_menu.getSelectedId();
      if (!m_plot_holder.empty()) {
        m_current_plot = &getPlotFromID<PlotAndTimer>(m_plot_holder, id);
        m_current_plot->plot->setVisible(true);
      }
      resized();
    };
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  };

  void resized() override {
    m_test_menu.setBounds(0, getScreenArea().getHeight() / 30, getWidth() / 2,
                          getScreenArea().getHeight() / 30);
    m_menu_label.setBounds(0, 0, getWidth() / 2,
                           getScreenArea().getHeight() / 30);

    for (auto& plot : m_plot_holder) {
      if (plot.second.plot->isVisible()) {
        plot.second.plot->setBounds(
            0, getScreenArea().getHeight() / 15, getWidth(),
            getHeight() - getScreenArea().getHeight() / 15);
      }
    }
  };

  std::map<std::string, PlotAndTimer>* getPlotHolder() {
    return &m_plot_holder;
  }

  std::vector<std::shared_ptr<cmp::PlotLookAndFeel>> lnf;
  static inline std::map<std::string, std::shared_ptr<node>> head;

 private:
  std::string m_category;

  std::map<std::string, PlotAndTimer> m_plot_holder;
  PlotAndTimer* m_current_plot = nullptr;

  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestHandler)
};

static void add_test(
    std::function<void(juce::Component*, const std::string&)> new_fun_ptr,
    const std::string& test_name, const std::string& category) {
  static std::shared_ptr<node> curr{nullptr};

  if (!TestHandler::head[category]) {
    TestHandler::head[category] = std::make_shared<node>();
    TestHandler::head[category]->test_item.test_function = new_fun_ptr;
    TestHandler::head[category]->test_item.test_name = test_name;
    curr = TestHandler::head[category];
  } else {
    curr->next = std::make_shared<node>();
    curr->next->test_item.test_function = new_fun_ptr;
    curr->next->test_item.test_name = test_name;
    curr = curr->next;
  }
}