#include "spl_plot.h"
#include <stdexcept>

#define FRAME_MARGIN static_cast<float>(getWidth() / 10)

PlotData::PlotData()
{
    wave_colour = juce::Colour(std::rand() % 100 + 100, std::rand() % 100 + 100, std::rand() % 100 + 100);
}

void PlotWindow::setPlotLimits()
{
    x_plot_limits.first  = getX() + FRAME_MARGIN;
    x_plot_limits.second = getX() + getWidth() - FRAME_MARGIN / 2;
    y_plot_limits.first  = getY() + FRAME_MARGIN;
    y_plot_limits.second = getY() + getHeight() - FRAME_MARGIN / 2;
}

bool PlotWindow::isValueWihtinPlotLimits(const float& x, const float& y) const
{
    if (x < x_plot_limits.first || x > x_plot_limits.second)
        return false;
    if (y < y_plot_limits.first || y > y_plot_limits.second)
        return false;

    return true;
}

void PlotWindow::xLim(const float& min, const float& max)
{
    if (min > max)
        throw std::invalid_argument("xLim min value must be lower than max value.");
    m_x_autoscale = false;
    m_x_scale     = static_cast<float>(getWidth() - FRAME_MARGIN * 1.5) / (max - min);
}

void PlotWindow::yLim(const float& min, const float& max)
{
    if (min > max)
        throw std::invalid_argument("yLim min value must be lower than max value.");
    m_y_autoscale = false;
    m_y_scale     = static_cast<float>(getHeight() - FRAME_MARGIN * 1.5) / (max - min);
    m_y_offset    = min;
}

struct MinMaxValues
{
    float min_value;
    float max_value;
    size_t y_max_size;
};

static std::pair<float, float> findXMinMaxValues(const std::vector<PlotData_ptr>& plots_data)
{
    auto max_value = std::numeric_limits<float>::min();
    auto min_value = std::numeric_limits<float>::max();

    for (const auto& single_plot : plots_data)
    {
        const auto& current_max = *std::max_element((*single_plot.x_values).begin(), (*single_plot.x_values).end());
        max_value               = current_max > max_value ? current_max : max_value;
        const auto& current_min = *std::min_element((*single_plot.x_values).begin(), (*single_plot.x_values).end());
        min_value               = current_min < min_value ? current_min : min_value;
    }
    return {min_value, max_value};
}

static std::pair<float, float> findYMinMaxValues(const std::vector<PlotData_ptr>& plots_data)
{
    auto max_value = std::numeric_limits<float>::min();
    auto min_value = std::numeric_limits<float>::max();

    for (const auto& single_plot : plots_data)
    {
        const auto& current_max = *std::max_element((*single_plot.y_values).begin(), (*single_plot.y_values).end());
        max_value               = current_max > max_value ? current_max : max_value;
        const auto& current_min = *std::min_element((*single_plot.y_values).begin(), (*single_plot.y_values).end());
        min_value               = current_min < min_value ? current_min : min_value;
    }
    return {min_value, max_value};
}

void PlotWindow::setAutoXScale()
{
    const auto [min_value, max_value] = findXMinMaxValues(m_plots_data_ptr);

    if (abs(max_value - min_value) > std::numeric_limits<float>::epsilon())
    {
        m_x_scale  = (static_cast<float>(getWidth() - FRAME_MARGIN * 1.5) / (max_value - min_value));
        m_x_offset = min_value;
    }
    else
    {
        throw std::invalid_argument("The min and max value of x_values must not be the same.");
    }
}

void PlotWindow::setAutoYScale()
{
    const auto [min_value, max_value] = findYMinMaxValues(m_plots_data_ptr);

    if (abs(max_value - min_value) < std::numeric_limits<float>::epsilon())
    {
        m_y_scale = 0.f;
    }
    else
    {
        m_y_scale  = (static_cast<float>(getHeight() - FRAME_MARGIN * 1.5) / (max_value - min_value));
        m_y_offset = min_value;
    }
}

void PlotWindow::resized(){};

struct PathList
{
    std::vector<juce::Path> paths;
    juce::Colour colour;
};

void PlotWindow::paint(juce::Graphics& g)
{
    std::vector<PathList> wave_paths;

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(getX() + FRAME_MARGIN / 2,
                           getY() + FRAME_MARGIN / 2,
                           getWidth() - FRAME_MARGIN / 2,
                           getHeight() - FRAME_MARGIN / 2,
                           FRAME_MARGIN,
                           FRAME_MARGIN);

    if (m_x_autoscale)
    {
        setAutoXScale();
    }

    if (m_y_autoscale)
    {
        setAutoYScale();
    }

    const auto offset_y =
        static_cast<float>(getHeight()) + static_cast<float>(getY()) - FRAME_MARGIN / 2 + (m_y_offset * m_y_scale);
    const auto offset_x = static_cast<float>(getX()) + FRAME_MARGIN - (m_x_offset * m_x_scale);

    for (const auto& plot : m_plots_data_ptr)
    {
        bool start_new_path          = true;
        juce::Path* current_path_ptr = nullptr;
        std::vector<juce::Path> current_path;

        for (auto i = 0; i < (*plot.x_values).size(); ++i)
        {
            const auto& x = offset_x + ((*plot.x_values)[i] * m_x_scale);
            const auto& y = offset_y - ((*plot.y_values)[i] * m_y_scale);

            if (isValueWihtinPlotLimits(x, y) && start_new_path)
            {
                juce::Path wavePath;
                wavePath.startNewSubPath(x, y);
                current_path.push_back(wavePath);
                current_path_ptr = &current_path.back();
                start_new_path   = false;
            }
            else if (isValueWihtinPlotLimits(x, y))
            {
                current_path_ptr->lineTo(x, y);
            }
            else
            {
                start_new_path = true;
            }
        }
        wave_paths.push_back({current_path, *plot.wave_colour});
    }

    for (const auto& wp : wave_paths)
    {
        g.setColour(wp.colour);
        for (const auto& p : wp.paths)
        {
            g.strokePath(p,
                         juce::PathStrokeType(1.0f,
                                              juce::PathStrokeType::JointStyle::mitered,
                                              juce::PathStrokeType::EndCapStyle::rounded));
        }
    }
}

Plot::Plot(int x, int y, int width, int height)
{
    setBounds(x, y, width, height);
    setPlotLimits();
}

void Plot::plot(juce::Graphics& g, const std::vector<PlotData>& plots_data)
{
    if (plots_data.size() != m_plots_data_ptr.size())
    {
        m_plots_data_ptr.resize(plots_data.size());
    }

    size_t i = 0;
    for (const auto& plot : plots_data)
    {
        m_plots_data_ptr[i].y_values    = &plot.y_values;
        m_plots_data_ptr[i].wave_colour = &plot.wave_colour;
        if (plot.x_values.size() == 0 && m_x_values.empty())
        {
            m_x_values.push_back(std::vector<float>(plot.y_values.size()));
            m_plots_data_ptr[i].x_values = &m_x_values.back();
            std::iota(m_x_values.back().begin(), m_x_values.back().end(), 0);
        }
        else if (!plot.x_values.empty())
        {
            if (plot.x_values.size() != plot.y_values.size())
            {
                throw std::invalid_argument("x_values must have the same size as y_values. x_values has size: " +
                                            std::to_string(plot.x_values.size()) +
                                            " and y_values has size: " + std::to_string(plot.y_values.size()));
            }
            m_plots_data_ptr[i].x_values = &plot.x_values;
        }
        i++;
    }

    paint(g);
}
