#include "spl_grid.h"
#include <stdexcept>

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::dimgrey);
  for (const auto &path : m_grid_path) {
    g.strokePath(path, juce::PathStrokeType(1.0f));
  }
}

void BaseGrid::resized() { createGrid(); }

void Grid::createGrid() {
  const auto &width = static_cast<float>(getWidth());
  const auto &height = static_cast<float>(getHeight());

  auto num_horizontal_lines = 3;
  if (width > 375.f) {
    num_horizontal_lines = 11;
  } else if (width <= 375.f && width > 135.f) {
    num_horizontal_lines = 5;
  }

  auto num_vertical_lines = 3;
  if (width > 435.f) {
    num_vertical_lines = 11;
  } else if (width <= 435.f && width > 175.f) {
    num_vertical_lines = 5;
  }

  // Height 375 pixlar ska den gå från 11 till 5 linjer, 135 till 3 linjer
  // Width 435 pixlar gå från 11 till 5 linjer, 175 till 3 linjer
  m_grid_path.clear();

  for (int n_line = 0; n_line < num_horizontal_lines; ++n_line) {
    const auto &y_pos = height * (static_cast<float>(n_line) /
                                  static_cast<float>(num_horizontal_lines - 1));
    juce::Path path_grid;
    path_grid.startNewSubPath(getX(), getY() + y_pos);
    path_grid.lineTo(getX() + width, getY() + y_pos);
    m_grid_path.push_back(path_grid);
  }

  for (int n_line = 0; n_line < num_vertical_lines; ++n_line) {
    const auto &x_pos = width * (static_cast<float>(n_line) /
                                 static_cast<float>(num_vertical_lines - 1));
    juce::Path path_grid;
    path_grid.startNewSubPath(getX() + x_pos, getY());
    path_grid.lineTo(getX() + x_pos, getY() + height);
    m_grid_path.push_back(path_grid);
  }

  juce::PathStrokeType p_type(1.0f);
  std::vector<float> dashed_lines = {4, 7};

  for (auto &path : m_grid_path) {
    p_type.createDashedStroke(path, path, dashed_lines.data(), 2);
  }
}
