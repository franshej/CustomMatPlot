#pragma once

#include <spl_plot.h>

#include "test_utils.h"

struct PlotAndTimer {
  std::unique_ptr<scp::Plot> plot;
  std::unique_ptr<TimerCallback> timer;
};

class RTTestHandler : public juce::Component {
 public:
  RTTestHandler() : m_menu_label("", "Tests: ") {
    setSize(1200, 800);

    // Calls all the test functions created by the user.
    std::shared_ptr<node> cur;
    for (cur = RTTestHandler::head; cur; cur = cur->next) {
      cur->test_item.test_function(this, cur->test_item.test_name);
    }

    addAndMakeVisible(m_test_menu);
    addAndMakeVisible(m_menu_label);

    auto it = m_plot_holder.begin();
    for (auto i = 0u; i < m_plot_holder.size(); ++i) {
      m_test_menu.addItem((*it++).first, i + 1);
    }

    for (auto &plot : m_plot_holder) {
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

  void paint(juce::Graphics &g) override {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  };

  void resized() override {
    m_test_menu.setBounds(0, getScreenArea().getHeight() / 30, getWidth() / 2,
                          getScreenArea().getHeight() / 30);
    m_menu_label.setBounds(0, 0, getWidth() / 2,
                           getScreenArea().getHeight() / 30);

    for (auto &plot : m_plot_holder) {
      if (plot.second.plot->isVisible()) {
        plot.second.plot->setBounds(
            0, getScreenArea().getHeight() / 15, getWidth(),
            getHeight() - getScreenArea().getHeight() / 15);
      }
    }
  };

  std::map<std::string, PlotAndTimer> *get_plot_holder() {
    return &m_plot_holder;
  }

  std::vector<std::shared_ptr<scp::PlotLookAndFeel>> lnf;
  static inline std::shared_ptr<node> head;

 private:
  std::map<std::string, PlotAndTimer> m_plot_holder;
  PlotAndTimer *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RTTestHandler)
};