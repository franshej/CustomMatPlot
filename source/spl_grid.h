#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "spl_grid.h"

struct BaseGrid : juce::Component
{
public:
	BaseGrid() = default;
	~BaseGrid() = default;
	
	void resized() override;
	void paint(juce::Graphics& g) override;
	
private:
	virtual void createGrid() = 0;
	
protected:
	juce::Path m_grid_path;
};

struct Grid : BaseGrid
{
	void createGrid() override;
};

