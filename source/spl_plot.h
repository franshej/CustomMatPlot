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

struct PlotWindow : juce::Component
{
public:
    ~PlotWindow() = default;

    void xLim(const float& min, const float& max);
    void yLim(const float& min, const float& max);

    virtual void plot(juce::Graphics& g, const std::vector<PlotData>& plots_data) = 0;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    bool isValueWihtinPlotLimits(const float& x, const float& y) const;

protected:
    void setPlotLimits();
    void setAutoXScale();
    void setAutoYScale();

    float m_y_offset, m_x_offset, m_x_scale, m_y_scale;
    bool m_x_autoscale = true, m_y_autoscale = true;
    std::pair<float, float> x_plot_limits, y_plot_limits;

    std::vector<std::vector<float>> m_x_values;
    std::vector<PlotData_ptr> m_plots_data_ptr;
};

struct Plot : PlotWindow
{
public:
    Plot(const int x, const int y, const int width, const int height);
    ~Plot() = default;
    void plot(juce::Graphics& g, const std::vector<PlotData>& plots_data) override;
};

struct semiLogX : PlotWindow
{
public:
    semiLogX(const int x, const int y, const int width, const int height);
    void plot(juce::Graphics& g, const std::vector<PlotData>& plots_data) override;
};

