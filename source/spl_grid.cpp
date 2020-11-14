#include "spl_grid.h"
#include <stdexcept>

void BaseGrid::paint(juce::Graphics &g){
	g.setColour(juce::Colours::white);
	g.strokePath(m_grid_path, juce::PathStrokeType(1.0f));
}

void BaseGrid::resized(){
	createGrid();
}

void Grid::createGrid(){
	const auto& width = static_cast<float>(getWidth());
	const auto& height = static_cast<float>(getHeight());
	const auto& num_horizontal_lines = getHeight() / 50;
	const auto& num_vertical_lines = getWidth() / 50;
	
	for (int n_line = 0; n_line < num_horizontal_lines; ++n_line){
		const auto& y_pos = height * (static_cast<float>(n_line) / static_cast<float>(num_horizontal_lines));
		m_grid_path.startNewSubPath(getX(), getY() + y_pos);
		m_grid_path.lineTo(getX() + width, getY() + y_pos);
	}
}
