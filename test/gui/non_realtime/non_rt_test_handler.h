#pragma once

#include <spl_plot.h>
#include "test_utils.h"

class NonRTTestHandler : public juce::Component {
 public:
  NonRTTestHandler() : m_menu_label("", "Tests: ") {
    setSize(1200, 800);

    std::shared_ptr<node> cur;
    for (cur = NonRTTestHandler::head; cur; cur = cur->next) {
      cur->fun_ptr(this, cur->name);
    }

    addAndMakeVisible(m_test_menu);
    addAndMakeVisible(m_menu_label);

    auto it = m_plot_holder.begin();
    for (auto i = 0u; i < m_plot_holder.size(); ++i) {
      m_test_menu.addItem((*it++).first, i + 1);
    }

    for (auto &plot : m_plot_holder) {
      plot.second->setBounds(0, getScreenArea().getHeight() / 15, getWidth(),
                             getHeight() - getScreenArea().getHeight() / 15);
      plot.second->setVisible(false);
    }

    m_test_menu.onChange = [this]() {
      if (m_current_plot != nullptr) {
        m_current_plot->setVisible(false);
      }
      const auto id = m_test_menu.getSelectedId();
      if (!m_plot_holder.empty()) {
        m_current_plot = getPlotFromID(m_plot_holder, id);
        m_current_plot->setVisible(true);
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
      if (plot.second->isVisible()) {
        plot.second->setBounds(0, getScreenArea().getHeight() / 15, getWidth(),
                               getHeight() - getScreenArea().getHeight() / 15);
      }
    }
  };

  std::map<std::string, std::unique_ptr<scp::Plot>> *get_plot_holder() {
    return &m_plot_holder;
  }

  std::vector<std::shared_ptr<scp::PlotLookAndFeel>> lnf;
  static inline std::shared_ptr<node> head;

 private:
  std::map<std::string, std::unique_ptr<scp::Plot>> m_plot_holder;
  scp::Plot *m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NonRTTestHandler)
};

static void add_test(
    std::function<void(juce::Component *, const std::string &)> new_fun_ptr,
    const std::string &test_name) {
  static std::shared_ptr<node> curr{nullptr};

  if (!NonRTTestHandler::head) {
    NonRTTestHandler::head = std::make_shared<node>();
    NonRTTestHandler::head->fun_ptr = new_fun_ptr;
    NonRTTestHandler::head->name = test_name;
    curr = NonRTTestHandler::head;
  } else {
    curr->next = std::make_shared<node>();
    curr->next->fun_ptr = new_fun_ptr;
    curr->next->name = test_name;
    curr = curr->next;
  }
}