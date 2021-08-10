#include "spl_grid.h"

#include <stdexcept>

/*============================================================================*/

template <class float_type>
static std::string convertFloatToString(const float_type value,
                                        int num_decimals, int max_string_len) {
  if (!(std::is_same<float, float_type>::value ||
        std::is_same<double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto pow_of_ten =
      value == 0.f ? 0 : static_cast<int>(floor(log10(abs(value))));
  const auto is_neg = static_cast<int>(value < 0);

  auto text_out = std::to_string(value);

  const auto len_before_dec = pow_of_ten < 0 ? abs(pow_of_ten) : pow_of_ten + 1;
  const auto req_len = len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (max_string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto two_decimals =
          text_out.substr(is_neg + pow_of_ten + int(1), 3);
      const auto first_digit = text_out.substr(0, int(1) + is_neg);
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals =
          text_out.substr(len_before_dec + is_neg + int(1), 4);
      three_decimals.insert(0, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else {
    text_out =
        text_out.substr(0, len_before_dec + is_neg + int(1) + num_decimals);
  }

  return text_out;
}

static std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator &custom_labels_it) {
  return *(custom_labels_it++);
}

template <class num_type>
static std::tuple<num_type, num_type, num_type, num_type> getRectangleMeasures(
    juce::Rectangle<int> graph_area) {
  const auto x = static_cast<num_type>(graph_area.getX());
  const auto y = static_cast<num_type>(graph_area.getY());
  const auto width = static_cast<num_type>(graph_area.getWidth());
  const auto height = static_cast<num_type>(graph_area.getHeight());
  return std::make_tuple(x, y, width, height);
}

template <class graph_type>
static GraphLine *getAndAddGridLine(
    std::vector<std::unique_ptr<GraphLine>> &graph_lines,
    const juce::Colour grid_colour) {
  graph_lines.emplace_back(std::make_unique<graph_type>(grid_colour));
  return graph_lines.back().get();
}

/*============================================================================*/

void BaseGrid::createLabels(const std::function<float(const float)> xToXPos,
                            const std::function<float(const float)> yToYPos) {
  const auto [x, y, width, height] =
      getRectangleMeasures<float>(m_config_params.graph_area);

  const auto font = juce::Font(m_graphic_params.font);

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
                  const auto font_height = font.getHeightInPoints();

                  const auto x_label_area = juce::Rectangle<int>(
                      x + xToXPos(x_val[0]) - x_label_width / 2,
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
                  const auto font_height = font.getHeightInPoints();

                  auto y_label_area = juce::Rectangle<int>(
                      x - font_height - y_label_width,
                      y + yToYPos(y_val[0]) - font_height / 2, y_label_width,
                      font_height);

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
  params.text_colour = juce::Colours::white;
  params.frame_colour = juce::Colours::white;
  params.font = juce::Font("Arial Rounded MT", 16.f, juce::Font::plain);

  return params;
}

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(m_graphic_params.text_colour);
  g.setFont(m_graphic_params.font);
  for (const auto &y_axis_text : m_y_axis_labels) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredRight);
    g.drawRoundedRectangle(
        {float(y_axis_text.second.getX()), float(y_axis_text.second.getY()),
         float(y_axis_text.second.getWidth()),
         float(y_axis_text.second.getHeight())},
        1.f, 1.f);
  }
  for (const auto &x_axis_text : m_x_axis_labels) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
    g.drawRoundedRectangle(
        {float(x_axis_text.second.getX()), float(x_axis_text.second.getY()),
         float(x_axis_text.second.getWidth()),
         float(x_axis_text.second.getHeight())},
        1.f, 1.f);
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

  if (!m_config_params.graph_area) {
    jassert(
        "Make sure that the m_config_params.graph_area is set. Use "
        "'setGraphBounds'");
    return;
  }

  m_frame = std::make_unique<FrameComponent>(m_graphic_params.frame_colour);
  m_frame->setBounds(m_config_params.graph_area);
  addAndMakeVisible(m_frame.get(), -1);

  prepareDataHolders(m_vertical_grid_lines, m_horizontal_grid_lines);

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
      getRectangleMeasures<float>(m_config_params.graph_area);

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

void BaseGrid::setGraphBounds(const juce::Rectangle<int> &graph_area) {
  m_config_params.graph_area = graph_area;
  resized();
}

void BaseGrid::setYLim(const float min, const float max) {
  m_config_params.y_lim = {min, max};
}

void BaseGrid::setXLim(const float min, const float max) {
  m_config_params.x_lim = {min, max};
}

void BaseGrid::setGridON(const bool grid_on) {
  m_config_params.grid_on = grid_on;
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
      getRectangleMeasures<float>(m_config_params.graph_area);

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);

  auto GridLines = getAndAddGridLine<graph_type>(m_vertical_grid_lines,
                                                 m_graphic_params.grid_colour);
  GridLines->setBounds(m_config_params.graph_area);

  GridLines->xLim(x_lim.min, x_lim.max);
  GridLines->yLim(0.f, height);

  GridLines->setXValues({x_val, x_val});
  GridLines->setYValues({0.f, height});

  const auto font = juce::Font(m_graphic_params.font);
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
      getRectangleMeasures<float>(m_config_params.graph_area);

  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  auto GridLines = getAndAddGridLine<graph_type>(m_horizontal_grid_lines,
                                                 m_graphic_params.grid_colour);
  GridLines->setBounds(m_config_params.graph_area);

  GridLines->xLim(0.f, width);
  GridLines->yLim(y_lim.min, y_lim.max);

  GridLines->setXValues({0.f, width});
  GridLines->setYValues({y_val, y_val});

  const auto font = juce::Font(m_graphic_params.font);
  const auto font_height = font.getHeightInPoints();
  if (!m_config_params.grid_on) {
    const std::vector<float> dashed_lines = {font_height, width - font_height,
                                             font_height};
    GridLines->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(GridLines, 0);
}

/*============================================================================*/

void Grid::prepareDataHolders(scp::GridLines &vertical_grid_lines,
                              scp::GridLines &horizontal_grid_lines) {
  const auto graph_area = juce::Rectangle<int>(m_config_params.graph_area);
  const unsigned width = graph_area.getWidth();
  const unsigned height = graph_area.getHeight();

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
      getRectangleMeasures<float>(m_config_params.graph_area);

  if (!(width > 0.f && height > 0.f)) return;

  vertical_scaling = scp::scaling::linear;
  horizontal_scaling = scp::scaling::linear;

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);
  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  // Create the vertical lines
  auto x_diff = (x_lim.max - x_lim.min) / float(m_num_vertical_lines);
  for (int i = 0; i <= m_num_vertical_lines; ++i) {
    const auto x_pos = x_lim.min + float(i) * x_diff;
    x_positions.push_back(x_pos);
  }

  // Then create the horizontal lines
  auto y_diff = (y_lim.max - y_lim.min) / float(m_num_horizontal_lines);
  for (int i = 0; i <= m_num_horizontal_lines; ++i) {
    const auto y_pos = y_lim.min + float(i) * y_diff;
    y_positions.push_back(y_pos);
  }
}

/*============================================================================*/

void SemiLogXGrid::prepareDataHolders(scp::GridLines &vertical_grid_lines,
                                      scp::GridLines &horizontal_grid_lines) {
  const auto graph_area = juce::Rectangle<int>(m_config_params.graph_area);
  const unsigned width = graph_area.getWidth();
  const unsigned height = graph_area.getHeight();

  m_num_vertical_lines = 10u;

  m_num_horizontal_lines = 3u;
  if (height > 375u) {
    m_num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    m_num_horizontal_lines = 5u;
  }

  const auto x_lim = scp::Lim_f(m_config_params.x_lim);
  const auto y_lim = scp::Lim_f(m_config_params.y_lim);

  m_min_exp = std::floor(log10(x_lim.min));
  m_max_exp = std::ceil(log10(x_lim.max));

  const auto top_out_of_sight_lines = (m_max_exp - log10(x_lim.max));
  const auto bottom_out_of_sight_lines = log10(x_lim.min) - m_min_exp;
  const auto num_out_of_sight_lines =
      top_out_of_sight_lines + bottom_out_of_sight_lines;

  m_exp_diff = static_cast<unsigned>(ceil(abs(m_max_exp) - abs(m_min_exp)));

  m_num_lines_exp = 10;

  m_num_vertical_lines = (m_exp_diff * (m_num_lines_exp)) -
                         (num_out_of_sight_lines * m_num_lines_exp);

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
      getRectangleMeasures<float>(m_config_params.graph_area);

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
  for (int i = 0; i <= m_num_horizontal_lines; ++i) {
    const auto y_val = y_lim.min + float(i) * y_diff;
    y_positions.push_back(y_val);
  }
}
