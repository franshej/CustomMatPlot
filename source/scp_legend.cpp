#include "scp_legend.h"
#include "spl_graph_line.h"

#include "spl_plot.h"

void scp::Legend::setLegend(const std::vector<std::string>& label_texts) {
  m_label_texts = label_texts;
}

void scp::Legend::setDataSeries(const GraphLines* data_series) {
  m_graph_lines = data_series;
}

void scp::Legend::resized() {}

void scp::Legend::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<PlotBase::LookAndFeelMethods*>(m_lookandfeel);

    auto getColourVector =
        [](const auto* graph_lines) -> std::vector<juce::Colour> {
      std::vector<juce::Colour> retval;
      for (const auto &graph : (*graph_lines)) {
        retval.emplace_back(graph->getColour());
      }
      return retval;
    };
    lnf->drawLegend(g, m_label_texts, getColourVector(m_graph_lines),
                    getBounds());
  }
}

void scp::Legend::lookAndFeelChanged() {
  if (auto* lnf =
          dynamic_cast<PlotBase::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
