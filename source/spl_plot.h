#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "spl_graph_line.h"
#include "spl_grid.h"

struct Plot : juce::Component
{
public:
	Plot();
	~Plot() = default;
	
	void xLim(const float& min, const float& max);
	void yLim(const float& min, const float& max);
	
	virtual void updateYData(const std::vector<std::vector<float>> &y_data) = 0;
	virtual void updateXData(const std::vector<std::vector<float>> &x_data) = 0;
	
	void resized() override;
	void paint(juce::Graphics& g) override;
	
protected:
	void setAutoXScale(const std::vector<std::vector<float>>& x_data);
	void setAutoYScale(const std::vector<std::vector<float>>& y_data);
	
	void updateYDataGraph();
	void updateXDataGraph();
	
	bool m_x_autoscale = true, m_y_autoscale = true;
	
	std::vector<std::unique_ptr<GraphLine>> m_graph_lines;
	juce::Rectangle<int> m_graph_area;
	std::vector<std::vector<float>> m_y_data, m_x_data;
	std::unique_ptr<BaseGrid> m_grid;
};

struct LinearPlot : Plot
{
	public:
		~LinearPlot() = default;
		LinearPlot(const int x, const int y, const int width, const int height);
		LinearPlot();
		void updateYData(const std::vector<std::vector<float>> &y_data) override;
	  void updateXData(const std::vector<std::vector<float>> &x_data) override;
};

