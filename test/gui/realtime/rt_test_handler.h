#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

static scp::Plot *getPlotFromID(
    std::map<std::string, std::unique_ptr<scp::Plot>> &plot_holder,
    const int id) {
  if (id >= plot_holder.size()) return nullptr;

  auto it = plot_holder.begin();

  for (int i = 1; i < plot_holder.size() + 1; ++i) {
    if (id == i) break;
    it++;
  }

  return it->second.get();
}

static juce::Rectangle<int> getScreenArea() {
  return juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
}

struct node {
  std::function<void(juce::Component *, const std::string &)> fun_ptr = nullptr;
  std::string name;
  std::shared_ptr<node> next = nullptr;
};

class RTTestHandler : public juce::Component {
 public:
  RTTestHandler() : m_menu_label("", "Tests: ") {
    setSize(1200, 800);

    std::shared_ptr<node> cur;
    for (cur = RTTestHandler::head; cur; cur = cur->next) {
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RTTestHandler)
};

static void add_test(
    std::function<void(juce::Component *, const std::string &)> new_fun_ptr,
    const std::string &test_name) {
  static std::shared_ptr<node> curr{nullptr};

  if (!RTTestHandler::head) {
    RTTestHandler::head = std::make_shared<node>();
    RTTestHandler::head->fun_ptr = new_fun_ptr;
    RTTestHandler::head->name = test_name;
    curr = RTTestHandler::head;
  } else {
    curr->next = std::make_shared<node>();
    curr->next->fun_ptr = new_fun_ptr;
    curr->next->name = test_name;
    curr = curr->next;
  }
}