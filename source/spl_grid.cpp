#include "spl_grid.h"

#include <stdexcept>
#include <tuple>

/*============================================================================*/

template <class float_type>
constexpr static std::string convertFloatToString(const float_type value,
                                                  std::size_t num_decimals,
                                                  std::size_t max_string_len) {
  if (!(std::is_same<float, float_type>::value ||
        std::is_same<double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto pow_of_ten = value == 0.f ? 0 : int(floor(log10(abs(value))));
  const auto is_neg = std::size_t(value < 0);

  auto text_out = std::to_string(value);

  const auto len_before_dec = pow_of_ten < 0
                                  ? std::size_t(abs(float(pow_of_ten)))
                                  : std::size_t(pow_of_ten) + 1u;
  const auto req_len = len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (max_string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto two_decimals =
          text_out.substr(is_neg + std::size_t(pow_of_ten) + 1u, 3);
      const auto first_digit = text_out.substr(0, 1u + is_neg);
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals = text_out.substr(len_before_dec + is_neg + 1u, 4);
      three_decimals.insert(0, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else {
    text_out = text_out.substr(0, len_before_dec + is_neg + 1u + num_decimals);
  }

  return text_out;
}

static std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator &custom_labels_it) {
  return *(custom_labels_it++);
}

template <class graph_type>
constexpr static GraphLine *getAndAddGridLine(
    std::vector<std::unique_ptr<GraphLine>> &graph_lines,
    const juce::Colour grid_colour) {
  graph_lines.emplace_back(std::make_unique<graph_type>(grid_colour));
  return graph_lines.back().get();
}

/*============================================================================*/

void BaseGrid::createLabels(const std::function<float(const float)> xToXPos,
                            const std::function<float(const float)> yToYPos) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<int>(m_config_params.grid_area);

  const auto font = juce::Font(m_graphic_params.label_font);

  juce::Rectangle<int> *x_last_rect = nullptr;
  juce::Rectangle<int> *y_last_rect = nullptr;

  m_x_axis_labels.clear();
  m_y_axis_labels.clear();

  const auto use_custom_x_labels =
      m_custom_x_labels.size() >= m_vertical_grid_lines.size();

  auto m_custom_x_labels_reverse_it =
      use_custom_x_labels
          ? std::make_reverse_iterator(m_custom_x_labels.begin()) +
                m_custom_x_ticks.size() - m_vertical_grid_lines.size()
          : std::make_reverse_iterator(m_custom_x_labels.begin());

  std::for_each(std::make_reverse_iterator(m_vertical_grid_lines.end()),
                std::make_reverse_iterator(m_vertical_grid_lines.begin()),
                [&](const auto &grid) {
                  const auto x_val = grid->getXValues();

                  const std::string x_label =
                      use_custom_x_labels
                          ? getNextCustomLabel(m_custom_x_labels_reverse_it)
                          : convertFloatToString(x_val[0], 2, 6);

                  const auto x_label_width = font.getStringWidth(x_label);
                  const auto font_height = int(font.getHeightInPoints());

                  const auto x_label_area = juce::Rectangle<int>(
                      x + int(xToXPos(x_val[0])) - x_label_width / 2,
                      y + height + font_height, x_label_width, font_height);

                  if (!x_last_rect) {
                    m_x_axis_labels.push_back({x_label, x_label_area});
                    x_last_rect = &m_x_axis_labels.back().second;
                  } else {
                    if (!x_last_rect->intersects(x_label_area)) {
                      m_x_axis_labels.push_back({x_label, x_label_area});
                      x_last_rect = &m_x_axis_labels.back().second;
                    }
                  }
                });

  const auto use_custom_y_labels =
      m_custom_y_labels.size() >= m_horizontal_grid_lines.size();

  auto m_custom_y_labels_reverse_it =
      use_custom_y_labels
          ? std::make_reverse_iterator(m_custom_y_labels.begin()) +
                m_custom_y_ticks.size() - m_horizontal_grid_lines.size()
          : std::make_reverse_iterator(m_custom_y_labels.begin());

  std::for_each(std::make_reverse_iterator(m_horizontal_grid_lines.end()),
                std::make_reverse_iterator(m_horizontal_grid_lines.begin()),
                [&](const auto &grid) {
                  const auto y_val = grid->getYValues();

                  const std::string y_label =
                      use_custom_y_labels
                          ? getNextCustomLabel(m_custom_y_labels_reverse_it)
                          : convertFloatToString(y_val[0], 2, 6);

                  const auto y_label_width = font.getStringWidth(y_label);
                  const auto font_height = int(font.getHeightInPoints());

                  auto y_label_area = juce::Rectangle<int>(
                      x - font_height - y_label_width,
                      y + int(yToYPos(y_val[0])) - font_height / 2,
                      y_label_width, font_height);

                  if (!y_last_rect) {
                    m_y_axis_labels.push_back({y_label, y_label_area});
                    y_last_rect = &m_y_axis_labels.back().second;
                  } else {
                    if (!y_last_rect->intersects(y_label_area)) {
                      m_y_axis_labels.push_back({y_label, y_label_area});
                      y_last_rect = &m_y_axis_labels.back().second;
                    }
                  }
                });
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

void BaseGrid::setXLabels(const std::vector<std::string> &x_labels) {
  m_custom_x_labels = x_labels;
}

void BaseGrid::resized() {
  if (!m_config_params.x_lim) {
    jassert("x limit must be set. USe 'SetXLim'");
    return;
  }

  if (!m_config_params.y_lim) {
    jassert("y limit must be set. Use 'setYLim'");
    return;
  }

  if (!m_config_params.grid_area) {
    jassert(
        "Make sure that the m_config_params.grid_area is set. Use "
        "'setGridBounds'");
    return;
  }

  m_frame =
      std::make_unique<scp::FrameComponent>(m_graphic_params.frame_colour);
  m_frame->setBounds(m_config_params.grid_area);
  addAndMakeVisible(m_frame.get(), -1);

  prepareGridContainers(m_vertical_grid_lines, m_horizontal_grid_lines,
                        m_config_params.tiny_grid_on);

  std::vector<float> x_auto_ticks, y_auto_ticks;
  scp::scaling vertical_scaling, horizontal_scaling;
  createGrid(x_auto_ticks, y_auto_ticks, vertical_scaling, horizontal_scaling);

  const auto &x_ticks =
      m_custom_x_ticks.empty() ? x_auto_ticks : m_custom_x_ticks;
  const auto &y_ticks =
      m_custom_y_ticks.empty() ? y_auto_ticks : m_custom_y_ticks;

  for (const auto x_val : x_ticks) {
    if (vertical_scaling == scp::scaling::logarithmic)
      addGridLineVertical<LogXGraphLine>(x_val);
    else
      addGridLineVertical<LinearGraphLine>(x_val);
  }

  for (const auto y_val : y_ticks) {
    if (horizontal_scaling == scp::scaling::logarithmic) {
      // TODO: add logscale for y-values.
      jassert("'LogYGraphLine' is not implemented.");
    } else
      addGridLineHorizontal<LinearGraphLine>(y_val);
  }

  for (const auto &grid : m_vertical_grid_lines) {
    if (grid) {
      grid->calculateXData();
      grid->calculateYData();
    }
  }

  for (const auto &grid : m_horizontal_grid_lines) {
    if (grid) {
      grid->calculateXData();
      grid->calculateYData();
    }
  }
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  const auto valToPostion =
      [](const scp::Lim_f &lim, const float measure, const scp::scaling scale,
         const bool is_vertical) -> std::function<float(const float)> {
    if (scale == scp::scaling::logarithmic)
      return [=](const float val) -> float {
        return measure * (log(val / lim.min) / log(lim.max / lim.min));
      };

    return [=](const float val) -> float {
      const auto pos = measure * (lim.max - val) / (lim.max - lim.min);
      return is_vertical ? measure - pos : pos;
    };
  };

  const auto xToXPos =
      valToPostion(m_config_params.x_lim, width, vertical_scaling, true);
  const auto yToYPos =
      valToPostion(m_config_params.y_lim, height, horizontal_scaling, false);

  createLabels(xToXPos, yToYPos);
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

  auto GridLines = getAndAddGridLine<graph_type>(m_vertical_grid_lines,
                                                 m_graphic_params.grid_colour);
  GridLines->setBounds(m_config_params.grid_area);

  GridLines->setXLim(x_lim.min, x_lim.max);
  GridLines->setYLim(0.f, height);

  GridLines->setXValues({x_val, x_val});
  GridLines->setYValues({0.f, height});

  const auto font = juce::Font(m_graphic_params.label_font);
  const auto font_height = font.getHeightInPoints();

  if (!m_config_params.grid_on) {
    const std::vector<float> dashed_lines = {font_height, height - font_height,
                                             font_height};
    GridLines->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(GridLines, 0);
}

template <class graph_type>
void BaseGrid::addGridLineHorizontal(const float y_val) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  auto GridLines = getAndAddGridLine<graph_type>(m_horizontal_grid_lines,
                                                 m_graphic_params.grid_colour);
  GridLines->setBounds(m_config_params.grid_area);

  GridLines->setXLim(0.f, width);
  GridLines->setYLim(y_lim.min, y_lim.max);

  GridLines->setXValues({0.f, width});
  GridLines->setYValues({y_val, y_val});

  const auto font = juce::Font(m_graphic_params.label_font);
  const auto font_height = font.getHeightInPoints();
  if (!m_config_params.grid_on) {
    const std::vector<float> dashed_lines = {font_height, width - font_height,
                                             font_height};
    GridLines->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(GridLines, 0);
}

/*============================================================================*/

void Grid::prepareGridContainers(GridLines &vertical_grid_lines,
                                 GridLines &horizontal_grid_lines,
                                 const bool &tiny_grid_on) {
  const auto grid_area = juce::Rectangle<int>(m_config_params.grid_area);
  const unsigned width = grid_area.getWidth();
  const unsigned height = grid_area.getHeight();

  m_num_vertical_lines = 3u;
  if (width > 435u) {
    m_num_vertical_lines = 11u;
  } else if (width <= 435u && width > 175u) {
    m_num_vertical_lines = 5u;
  }

  m_num_horizontal_lines = 3u;
  if (height > 375u) {
    m_num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    m_num_horizontal_lines = 5u;
  }

  if (tiny_grid_on) {
    m_num_vertical_lines *= 2;
    m_num_horizontal_lines *= 2;
  }

  if (tiny_grid_on) {
    m_num_vertical_lines *= 2;
    m_num_horizontal_lines *= 2;
  }

  vertical_grid_lines.clear();
  vertical_grid_lines.reserve(m_num_vertical_lines);

  horizontal_grid_lines.clear();
  horizontal_grid_lines.reserve(m_num_horizontal_lines);
}

void Grid::createGrid(std::vector<float> &x_positions,
                      std::vector<float> &y_positions,
                      scp::scaling &vertical_scaling,
                      scp::scaling &horizontal_scaling) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  if (!(width > 0.f && height > 0.f)) return;

  vertical_scaling = scp::scaling::linear;
  horizontal_scaling = scp::scaling::linear;

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);
  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  // Create the vertical lines
  auto x_diff = (x_lim.max - x_lim.min) / float(m_num_vertical_lines);
  for (std::size_t i = 0; i != m_num_vertical_lines + 1u; ++i) {
    const auto x_pos = x_lim.min + float(i) * x_diff;
    x_positions.push_back(x_pos);
  }

  // Then create the horizontal lines
  auto y_diff = (y_lim.max - y_lim.min) / float(m_num_horizontal_lines);
  for (std::size_t i = 0; i != m_num_horizontal_lines + 1u; ++i) {
    const auto y_pos = y_lim.min + float(i) * y_diff;
    y_positions.push_back(y_pos);
  }
}

/*============================================================================*/

void SemiLogXGrid::prepareGridContainers(GridLines &vertical_grid_lines,
                                         GridLines &horizontal_grid_lines,
                                         const bool &tiny_grid_on) {
  const auto grid_area = juce::Rectangle<int>(m_config_params.grid_area);
  const unsigned width = grid_area.getWidth();
  const unsigned height = grid_area.getHeight();

  m_num_horizontal_lines = 3u;
  if (height > 375u) {
    m_num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    m_num_horizontal_lines = 5u;
  }

  m_num_lines_exp = 3u;
  if (width > 435u) {
    m_num_lines_exp = 10u;
  } else if (width <= 435u && width > 175u) {
    m_num_lines_exp = 5u;
  }

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);
  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  m_min_exp = std::floor(log10(x_lim.min));
  m_max_exp = std::ceil(log10(x_lim.max));

  const auto top_out_of_sight_lines = (m_max_exp - log10(x_lim.max));
  const auto bottom_out_of_sight_lines = log10(x_lim.min) - m_min_exp;
  const auto num_out_of_sight_lines =
      top_out_of_sight_lines + bottom_out_of_sight_lines;

  m_exp_diff = ceil(abs(m_max_exp) - abs(m_min_exp));

  m_num_vertical_lines =
      std::size_t((m_exp_diff * m_num_lines_exp) -
                  (num_out_of_sight_lines * m_num_lines_exp));

  if (tiny_grid_on) {
    m_num_vertical_lines *= 2;
    m_num_lines_exp *= 2;
    m_num_horizontal_lines *= 2;
  }

  vertical_grid_lines.clear();
  vertical_grid_lines.reserve(m_num_vertical_lines);

  horizontal_grid_lines.clear();
  horizontal_grid_lines.reserve(m_num_horizontal_lines);
}

void SemiLogXGrid::createGrid(std::vector<float> &x_positions,
                              std::vector<float> &y_positions,
                              scp::scaling &vertical_scaling,
                              scp::scaling &horizontal_scaling) {
  const auto [x, y, width, height] =
      scp::getRectangleMeasures<float>(m_config_params.grid_area);

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);
  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  vertical_scaling = scp::scaling::logarithmic;
  horizontal_scaling = scp::scaling::linear;

  // Frist create the vertical lines
  for (float curr_exp = m_min_exp; curr_exp < m_max_exp; ++curr_exp) {
    const auto curr_x_pos_base = pow(10.f, curr_exp);

    const auto x_diff =
        pow(10.f, curr_exp + 1.f) / static_cast<float>(m_num_lines_exp);
    for (float line = 0; line < m_num_lines_exp; ++line) {
      const auto x_val = curr_x_pos_base + line * x_diff;
      x_positions.push_back(x_val);
    }
  }

  // Then create the horizontal lines
  auto y_diff = (y_lim.max - y_lim.min) / float(m_num_horizontal_lines);
  for (std::size_t i = 0; i != std::size_t(m_num_horizontal_lines + 1u); ++i) {
    const auto y_val = y_lim.min + float(i) * y_diff;
    y_positions.push_back(y_val);
  }
}
