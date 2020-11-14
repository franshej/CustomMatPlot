#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct PlotData
{
public:
    PlotData();
    std::vector<float> x_values;
    std::vector<float> y_values;
    juce::Colour wave_colour;
};

struct PlotData_ptr
{
    const std::vector<float>* x_values;
    const std::vector<float>* y_values;
    const juce::Colour* wave_colour;
};

struct GraphLine : juce::Component
{
public:
		GraphLine();
    ~GraphLine() = default;

    void xLim(const float& min, const float& max);
    void yLim(const float& min, const float& max);
		void updateYValues(const std::vector<float> *y_values);
		void updateXValues(const std::vector<float> *x_values);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    bool isValueWihtinPlotLimits(const float& x, const float& y) const;
		virtual void calculateYData() = 0;
		virtual void calculateXData() = 0;

protected:
    void setPlotLimits();
    void setAutoXScale();
    void setAutoYScale();

	  float m_x_min, m_x_max, m_y_min, m_y_max;
    bool m_x_autoscale = true, m_y_autoscale = true;
		std::atomic<bool> nextYBlockReady, nextXBlockReady;
    std::pair<float, float> x_plot_limits, y_plot_limits;

    std::vector<std::vector<float>> m_x_values;
    std::vector<PlotData_ptr> m_plots_data_ptr;
	//	std::vector<PlotData> m_plot_data;
	
		const std::vector<float>* m_y_data;
		const std::vector<float>* m_x_data;
		std::vector<juce::Point<float>> m_graph_points;
	
		const juce::Colour m_graph_colour;
};

struct LinearGraphLine : GraphLine
{
public:
		//LinearGraphLine() = default;
    ~LinearGraphLine() = default;
	
private:
		void calculateYData() override;
    void calculateXData() override;
};

struct LogXGraphLine : GraphLine
{
public:
    LogXGraphLine(const int x, const int y, const int width, const int height);
};

