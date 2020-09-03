#include "MainComponent.h"
#define PI2 6.28318530718

MainComponent::MainComponent()
    : test_plot1(0, 0, 200, 100)
    , test_plot2(200, 0, 200, 100)
    , test_plot3(0, 100, 200, 100)
    , test_plot4(200, 100, 200, 100)

{
    setSize(600, 400);
}

void test_simple_ramp(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(10);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    Plot plot(0, 200, 200, 100);
    plot.plot(g, test_data);
}

void test_sinus_auto_lim(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size());
    }
    Plot plot(200, 200, 200, 100);
    plot.plot(g, test_data);
}

void test_two_sinus_auto_lim(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size());
    }

    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    test_data.back().x_values = std::vector<float>(100);
    std::iota(test_data.back().x_values.begin(), test_data.back().x_values.end(), -50);

    for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size()) + 1.f;
    }
    Plot plot(400, 0, 200, 100);
    plot.plot(g, test_data);
}

void test_x_lim(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size());
    }
    test_data.back().x_values = std::vector<float>(100);
    std::iota(test_data.back().x_values.begin(), test_data.back().x_values.end(), -50);
    Plot plot(400, 100, 200, 100);
    plot.xLim(0, 100);
    plot.plot(g, test_data);
}

void test_y_lim(juce::Graphics& g)
{
    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(100);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    for (auto& y : test_data.back().y_values)
    {
        y = std::sin(y * PI2 / test_data.back().y_values.size());
    }
    Plot plot(400, 200, 200, 100);
    plot.yLim(-1, 0);
    plot.plot(g, test_data);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    std::vector<PlotData> test_data;
    test_data.push_back(PlotData());
    test_data.back().y_values = std::vector<float>(10);
    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);

    test_plot1.plot(g, test_data);

    test_data.back().y_values = std::vector<float>(10);
    test_plot2.plot(g, test_data);

    std::iota(test_data.back().y_values.begin(), test_data.back().y_values.end(), 0);
    test_data.back().x_values = std::vector<float>(10);
    std::iota(test_data.back().x_values.begin() + 5, test_data.back().x_values.end(), 0);
    test_data.back().x_values[0] = -3;
    test_plot3.plot(g, test_data);

    test_data.back().x_values = std::vector<float>(10);
    for (auto i = 0u; i < test_data.back().x_values.size(); ++i)
    {
        test_data.back().x_values[i] = i * 20;
    }
    test_data.back().x_values[0] = 3;
    test_plot4.plot(g, test_data);

    test_simple_ramp(g);
    test_sinus_auto_lim(g);
    test_two_sinus_auto_lim(g);
    test_x_lim(g);
    test_y_lim(g);
}

void MainComponent::resized() {}
