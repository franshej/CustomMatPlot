#include "spl_grid.h"

#include <stdexcept>
#include <tuple>

#include "spl_graph_line.h"
#include "spl_plot.h"
#include "spl_utils.h"

/*============================================================================*/

template <class graph_type>
constexpr static scp::GraphLine *getAndAddGridLine(
    std::vector<std::unique_ptr<scp::GraphLine>> &graph_lines) {
  graph_lines.emplace_back(
      std::make_unique<graph_type>(scp::Plot::GraphType::GridLine));
  return graph_lines.back().get();
}
/*============================================================================*/

namespace scp {

/*============================================================================*/

void BaseGrid::createLabels() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->createGridLabelsHorizontal(getBounds(), m_horizontal_grid_lines,
                                    m_custom_y_ticks, m_custom_y_labels,
                                    m_y_axis_labels);
    lnf->createGridLabelsVertical(getBounds(), m_vertical_grid_lines,
                                  m_custom_x_ticks, m_custom_x_labels,
                                  m_x_axis_labels);
  }
}

GridGraphicParams BaseGrid::createDefaultGraphicParams() const {
  GridGraphicParams params;

  params.grid_colour = juce::Colours::dimgrey;
  params.label_colour = juce::Colours::white;
  params.frame_colour = juce::Colours::white;
  params.label_font = juce::Font("Arial Rounded MT", 16.f, juce::Font::plain);

  return params;
}

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(m_graphic_params.label_colour);
  g.setFont(m_graphic_params.label_font);
  for (const auto &y_axis_text : m_y_axis_labels) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredRight);
  }
  for (const auto &x_axis_text : m_x_axis_labels) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
  }
}

void BaseGrid::lookAndFeelChanged() {
  if (auto *lnf =
          dynamic_cast<scp::Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
  for (auto &grid_line : m_horizontal_grid_lines) {
    grid_line->setLookAndFeel(&getLookAndFeel());
  }

  for (auto &grid_line : m_vertical_grid_lines) {
    grid_line->setLookAndFeel(&getLookAndFeel());
  }
}

void BaseGrid::setXLabels(const std::vector<std::string> &x_labels) {
  m_custom_x_labels = x_labels;
}

void BaseGrid::resized() {
  jassert_return(m_config_params.x_lim, "x limit must be set. Use 'SetXLim'");
  jassert_return(m_config_params.y_lim, "y limit must be set. Use 'setYLim'");
  jassert_return(getBounds().getWidth() > 0 && getBounds().getHeight() > 0.f,
                 "width and height must be larger than zero.");

  // Temp, should be removed
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    m_config_params.grid_area = lnf->getGraphBounds(getBounds());
  } else {
    return;
  }

  m_frame =
      std::make_unique<scp::FrameComponent>(m_graphic_params.frame_colour);
  m_frame->setBounds(m_config_params.grid_area);
  addAndMakeVisible(m_frame.get(), -1);

  std::vector<float> x_auto_ticks, y_auto_ticks;
  Scaling vertical_scaling{Plot::Scaling::linear},
      horizontal_scaling{Plot::Scaling::linear};

  setScaling(vertical_scaling, horizontal_scaling);

  if (m_custom_x_ticks.empty() || m_custom_y_ticks.empty())
    createAutoGridTicks(x_auto_ticks, y_auto_ticks, vertical_scaling,
                        horizontal_scaling);

  const auto &x_ticks =
      m_custom_x_ticks.empty() ? x_auto_ticks : m_custom_x_ticks;
  const auto &y_ticks =
      m_custom_y_ticks.empty() ? y_auto_ticks : m_custom_y_ticks;

  m_vertical_grid_lines.clear();
  m_horizontal_grid_lines.clear();

  for (const auto x_val : x_ticks) {
    if (vertical_scaling == Plot::Scaling::logarithmic)
      addGridLineVertical<scp::LogXGraphLine>(x_val);
    else
      addGridLineVertical<scp::LinearGraphLine>(x_val);
  }

  for (const auto y_val : y_ticks) {
    if (horizontal_scaling == Plot::Scaling::logarithmic) {
      // TODO: add logscale for y-values.
      jassert_return(false, "'LogYGraphLine' is not implemented.");
    } else
      addGridLineHorizontal<scp::LinearGraphLine>(y_val);
  }

  lookAndFeelChanged();

  for (const auto &grid : m_vertical_grid_lines) {
    if (grid) {
      grid->updateXGraphPoints();
      grid->updateYGraphPoints();
    }
  }

  for (const auto &grid : m_horizontal_grid_lines) {
    if (grid) {
      grid->updateXGraphPoints();
      grid->updateYGraphPoints();
    }
  }

  createLabels();
}

BaseGrid::BaseGrid(const GridGraphicParams &params)
    : m_graphic_params(createDefaultGraphicParams()) {
  if (params.frame_colour) m_graphic_params.frame_colour = params.frame_colour;
  if (params.grid_colour) m_graphic_params.grid_colour = params.grid_colour;
  if (params.label_colour) m_graphic_params.label_colour = params.label_colour;
  if (params.label_font) m_graphic_params.label_font = params.label_font;
}

BaseGrid::BaseGrid() : m_graphic_params(createDefaultGraphicParams()) {}

void BaseGrid::setGridBounds(const juce::Rectangle<int> &grid_area) {
  m_config_params.grid_area = grid_area;
  resized();
}

void BaseGrid::setYLim(const float min, const float max) {
  m_config_params.y_lim = {min, max};
}

void BaseGrid::setXLim(const float min, const float max) {
  m_config_params.x_lim = {min, max};
}

void BaseGrid::setGridON(const bool grid_on, const bool tiny_grids_on) {
  if (grid_on) m_config_params.grid_on = grid_on;
  if (tiny_grids_on) m_config_params.tiny_grid_on = tiny_grids_on;
}

void BaseGrid::setXTicks(const std::vector<float> &x_ticks) {
  m_custom_x_ticks = x_ticks;
}

void BaseGrid::setYLabels(const std::vector<std::string> &y_labels) {
  m_custom_y_labels = y_labels;
}

void BaseGrid::setYTicks(const std::vector<float> &y_ticks) {
  m_custom_y_ticks = y_ticks;
}

template <class graph_type>
void BaseGrid::addGridLineVertical(const float x_val) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);

  auto GridLine = getAndAddGridLine<graph_type>(m_vertical_grid_lines);
  GridLine->setBounds(m_config_params.grid_area);

  GridLine->setXLim(x_lim.min, x_lim.max);
  GridLine->setYLim(0.f, height);

  GridLine->setXValues({x_val, x_val});
  GridLine->setYValues({0.f, height});

  const auto font = juce::Font(m_graphic_params.label_font);
  const auto font_height = font.getHeightInPoints();

  if (!m_config_params.grid_on) {
    const std::vector<float> dashed_lines = {font_height, height - font_height,
                                             font_height};
    GridLine->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(GridLine, 0);
}

template <class graph_type>
void BaseGrid::addGridLineHorizontal(const float y_val) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  auto GridLine = getAndAddGridLine<graph_type>(m_horizontal_grid_lines);
  GridLine->setBounds(m_config_params.grid_area);

  GridLine->setXLim(0.f, width);
  GridLine->setYLim(y_lim.min, y_lim.max);

  GridLine->setXValues({0.f, width});
  GridLine->setYValues({y_val, y_val});

  const auto font = juce::Font(m_graphic_params.label_font);
  const auto font_height = font.getHeightInPoints();
  if (!m_config_params.grid_on) {
    const std::vector<float> dashed_lines = {font_height, width - font_height,
                                             font_height};
    GridLine->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(GridLine, 0);
}

void BaseGrid::createAutoGridTicks(std::vector<float> &x_ticks,
                                   std::vector<float> &y_ticks,
                                   Scaling vertical_scaling,
                                   Scaling horizontal_scaling) {
  if (m_lookandfeel) {
    if (auto *lnf =
            static_cast<scp::Plot::LookAndFeelMethods *>(m_lookandfeel)) {
      lnf->addVerticalGridLineTicksAuto(
          getBounds(), static_cast<Plot::Scaling>(vertical_scaling), x_ticks,
          m_config_params.x_lim);
      lnf->addHorizontalGridLineTicksAuto(
          getBounds(), static_cast<Plot::Scaling>(horizontal_scaling), y_ticks,
          m_config_params.y_lim);
    }
  }
}

/*============================================================================*/

void Grid::setScaling(Scaling &vertical_scaling, Scaling &horizontal_scaling) noexcept {
  vertical_scaling = Plot::Scaling::linear;
  horizontal_scaling = Plot::Scaling::linear;
}

/*============================================================================*/

void SemiLogXGrid::setScaling(Scaling &vertical_scaling,
                              Scaling &horizontal_scaling) noexcept {
  vertical_scaling = Plot::Scaling::logarithmic;
  horizontal_scaling = Plot::Scaling::linear;
}

}  // namespace scp