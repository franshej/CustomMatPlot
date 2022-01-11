#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

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

template <class TestHandler>
static void add_test(
    std::function<void(juce::Component*, const std::string&)> new_fun_ptr,
    const std::string& test_name, const int dt_ms = 0) {
  static std::shared_ptr<node> curr{nullptr};

  if (!TestHandler::head) {
    TestHandler::head = std::make_shared<node>();
    TestHandler::head->test_item.test_function = new_fun_ptr;
    TestHandler::head->test_item.test_name = test_name;
    TestHandler::head->test_item.dt_ms = dt_ms;
    curr = TestHandler::head;
  } else {
    curr->next = std::make_shared<node>();
    curr->next->test_item.test_function = new_fun_ptr;
    curr->next->test_item.test_name = test_name;
    curr->next->test_item.dt_ms = dt_ms;
    curr = curr->next;
  }
}