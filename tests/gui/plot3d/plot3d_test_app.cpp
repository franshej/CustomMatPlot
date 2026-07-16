/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/*
 * A small standalone GUI test app for cmp::Plot3D. It hosts several 3D test
 * scenes and shows one at a time; a scene can be chosen from the drop-down
 * menu or selected up front with a command-line argument:
 *
 *   example_app_plot3d_test <scene>
 *
 * where <scene> is a 1-based index or a (case-insensitive) substring of the
 * scene name, e.g. "3", "all log" or "linear".
 */

#include <cmp_plot3d.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace {

/** A row of Plot3D panels laid out evenly across the available width. */
class PlotRow : public juce::Component {
 public:
  cmp::Plot3D& add(std::unique_ptr<cmp::Plot3D> plot) {
    auto& ref = *plot;
    addAndMakeVisible(ref);
    m_plots.push_back(std::move(plot));
    return ref;
  }

  void resized() override {
    if (m_plots.empty()) return;

    const auto panel_width = getWidth() / static_cast<int>(m_plots.size());
    auto x = 0;
    for (auto& plot : m_plots) {
      plot->setBounds(x, 0, panel_width, getHeight());
      x += panel_width;
    }
  }

 private:
  std::vector<std::unique_ptr<cmp::Plot3D>> m_plots;
};

/*============================ Test data helpers ============================*/

struct Line3D {
  std::vector<float> x, y, z;
};

/** A helix winding up a linear z ramp spanning three decades (1 -> 1000). */
static Line3D makeHelix() {
  constexpr std::size_t num_points = 400u;
  constexpr auto num_turns = 4.0f;

  Line3D line;
  line.x.resize(num_points);
  line.y.resize(num_points);
  line.z.resize(num_points);

  for (std::size_t i = 0; i < num_points; ++i) {
    const auto t = float(i) / float(num_points - 1);
    const auto angle = t * num_turns * juce::MathConstants<float>::twoPi;

    line.x[i] = std::sin(angle);
    line.y[i] = std::cos(angle);
    line.z[i] = 1.0f + t * 999.0f;
  }

  return line;
}

/** A wavy curve with all-positive coordinates, valid on log axes: it climbs
 * the x-z diagonal (1 -> 1000) while y sweeps across three decades. */
static Line3D makeLogCurve() {
  constexpr std::size_t num_points = 400u;

  Line3D line;
  line.x.resize(num_points);
  line.y.resize(num_points);
  line.z.resize(num_points);

  for (std::size_t i = 0; i < num_points; ++i) {
    const auto t = float(i) / float(num_points - 1);
    const auto phase = 4.0f * juce::MathConstants<float>::pi * t;

    line.x[i] = std::pow(10.0f, 3.0f * t);
    line.y[i] = std::pow(10.0f, 1.5f + 1.4f * std::sin(phase));
    line.z[i] = std::pow(10.0f, 3.0f * t);
  }

  return line;
}

static void labelAxes(cmp::Plot3D& plot, const std::string& title) {
  plot.setTitle(title);
  plot.setXLabel("x");
  plot.setYLabel("y");
  plot.setZLabel("z");
}

/*=============================== Test scenes ===============================*/

/** The same helix on a linear and a logarithmic z-axis, side by side. */
static std::unique_ptr<juce::Component> makeLinearVsLogScene() {
  auto row = std::make_unique<PlotRow>();
  const auto helix = makeHelix();

  const std::vector<float> diag_z = {1.0f, 1000.0f};
  const Line3D diag{{-1.0f, 1.0f}, {-1.0f, 1.0f}, diag_z};
  const Line3D edge{{-1.0f, -1.0f}, {1.0f, 1.0f}, diag_z};

  const auto plot_both = [&](cmp::Plot3D& plot, const std::string& title) {
    plot.plot3({{.x = helix.x, .y = helix.y, .z = helix.z},
                {.x = diag.x, .y = diag.y, .z = diag.z},
                {.x = edge.x, .y = edge.y, .z = edge.z}});
    labelAxes(plot, title);
  };

  plot_both(row->add(std::make_unique<cmp::Plot3D>()), "Linear z-axis");
  plot_both(row->add(std::make_unique<cmp::Plot3D>(cmp::Scaling::linear,
                                                   cmp::Scaling::linear,
                                                   cmp::Scaling::logarithmic)),
            "Logarithmic z-axis");

  return row;
}

/** A single plot with all three axes logarithmic. */
static std::unique_ptr<juce::Component> makeAllLogScene() {
  auto row = std::make_unique<PlotRow>();
  const auto curve = makeLogCurve();

  const Line3D diag{{1.0f, 1000.0f}, {1.0f, 1000.0f}, {1.0f, 1000.0f}};
  const Line3D anti{{1.0f, 1000.0f}, {1000.0f, 1.0f}, {1.0f, 1000.0f}};

  auto& plot = row->add(std::make_unique<cmp::Plot3D>(
      cmp::Scaling::logarithmic, cmp::Scaling::logarithmic,
      cmp::Scaling::logarithmic));
  plot.plot3({{.x = curve.x, .y = curve.y, .z = curve.z},
              {.x = diag.x, .y = diag.y, .z = diag.z},
              {.x = anti.x, .y = anti.y, .z = anti.z}});
  labelAxes(plot, "x, y, z all logarithmic");

  return row;
}

/** A single plot with a linear x-axis and logarithmic y- and z-axes. */
static std::unique_ptr<juce::Component> makeMixedLogScene() {
  auto row = std::make_unique<PlotRow>();
  const auto curve = makeLogCurve();

  const Line3D diag{{1.0f, 1000.0f}, {1.0f, 1000.0f}, {1.0f, 1000.0f}};
  const Line3D anti{{1.0f, 1000.0f}, {1000.0f, 1.0f}, {1.0f, 1000.0f}};

  auto& plot = row->add(std::make_unique<cmp::Plot3D>(
      cmp::Scaling::linear, cmp::Scaling::logarithmic,
      cmp::Scaling::logarithmic));
  plot.plot3({{.x = curve.x, .y = curve.y, .z = curve.z},
              {.x = diag.x, .y = diag.y, .z = diag.z},
              {.x = anti.x, .y = anti.y, .z = anti.z}});
  labelAxes(plot, "x linear, y & z logarithmic");

  return row;
}

/** A single plain helix (linear axes). */
static std::unique_ptr<juce::Component> makeHelixScene() {
  auto row = std::make_unique<PlotRow>();
  const auto helix = makeHelix();

  auto& plot = row->add(std::make_unique<cmp::Plot3D>());
  plot.plot3({.x = helix.x, .y = helix.y, .z = helix.z});
  labelAxes(plot, "Helix");

  return row;
}

/*============================== Test handler ===============================*/

struct Scene {
  std::string name;
  std::function<std::unique_ptr<juce::Component>()> build;
};

class Plot3DTestHandler : public juce::Component {
 public:
  explicit Plot3DTestHandler(const juce::String& requested_scene) {
    m_scenes.push_back({"Helix: linear vs log z", makeLinearVsLogScene});
    m_scenes.push_back({"All axes logarithmic", makeAllLogScene});
    m_scenes.push_back({"x linear, y & z logarithmic", makeMixedLogScene});
    m_scenes.push_back({"Helix", makeHelixScene});

    setSize(1600, 800);

    m_menu_label.setText("Scene: ", juce::dontSendNotification);
    addAndMakeVisible(m_menu_label);
    addAndMakeVisible(m_menu);

    for (std::size_t i = 0; i < m_scenes.size(); ++i)
      m_menu.addItem(m_scenes[i].name, static_cast<int>(i) + 1);

    m_menu.onChange = [this] { showScene(m_menu.getSelectedId() - 1); };
    m_menu.setSelectedId(resolveRequestedScene(requested_scene) + 1);
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  }

  void resized() override {
    m_menu_label.setBounds(0, 0, 70, 30);
    m_menu.setBounds(70, 0, 400, 30);
    if (m_current) m_current->setBounds(0, 34, getWidth(), getHeight() - 34);
  }

 private:
  /** Resolve the command-line argument to a 0-based scene index: a number is
   * a 1-based index, otherwise the first scene whose name contains it. */
  int resolveRequestedScene(const juce::String& requested) const {
    const auto trimmed = requested.trim();
    if (trimmed.isEmpty()) return 0;

    if (trimmed.containsOnly("0123456789")) {
      const auto index = trimmed.getIntValue() - 1;
      if (index >= 0 && index < static_cast<int>(m_scenes.size())) return index;
      return 0;
    }

    for (std::size_t i = 0; i < m_scenes.size(); ++i) {
      if (juce::String(m_scenes[i].name).containsIgnoreCase(trimmed))
        return static_cast<int>(i);
    }

    return 0;
  }

  void showScene(const int index) {
    if (index < 0 || index >= static_cast<int>(m_scenes.size())) return;

    if (m_current) removeChildComponent(m_current.get());
    m_current = m_scenes[static_cast<std::size_t>(index)].build();
    addAndMakeVisible(*m_current);
    resized();
  }

  std::vector<Scene> m_scenes;
  std::unique_ptr<juce::Component> m_current;
  juce::Label m_menu_label;
  juce::ComboBox m_menu;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Plot3DTestHandler)
};

}  // namespace

//==============================================================================
class Plot3DTestApp : public juce::JUCEApplication {
 public:
  Plot3DTestApp() {}

  const juce::String getApplicationName() override {
    return JUCE_APPLICATION_NAME_STRING;
  }
  const juce::String getApplicationVersion() override {
    return JUCE_APPLICATION_VERSION_STRING;
  }
  bool moreThanOneInstanceAllowed() override { return true; }

  void initialise(const juce::String& commandLine) override {
    m_main_window.reset(new MainWindow(getApplicationName(), commandLine));
  }

  void shutdown() override { m_main_window = nullptr; }

  void systemRequestedQuit() override { quit(); }

  class MainWindow : public juce::DocumentWindow {
   public:
    MainWindow(juce::String name, const juce::String& commandLine)
        : DocumentWindow(
              name,
              juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                  ResizableWindow::backgroundColourId),
              DocumentWindow::allButtons),
          m_handler(commandLine) {
      setUsingNativeTitleBar(true);
      setContentNonOwned(&m_handler, true);
      setResizable(true, true);
      centreWithSize(m_handler.getWidth(), m_handler.getHeight());
      setVisible(true);
    }

    void closeButtonPressed() override {
      JUCEApplication::getInstance()->systemRequestedQuit();
    }

   private:
    Plot3DTestHandler m_handler;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
  };

 private:
  std::unique_ptr<MainWindow> m_main_window;
};

//==============================================================================
START_JUCE_APPLICATION(Plot3DTestApp)
