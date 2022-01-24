#include "cmp_legend.h"
#include "cmp_graph_line.h"

#include "cmp_plot.h"

void cmp::Legend::setLegend(const std::vector<std::string>& label_texts) {
  m_label_texts = label_texts;
}

void cmp::Legend::setDataSeries(const GraphLines* data_series) {
  m_graph_lines = data_series;
}

void cmp::Legend::resized() {}

void cmp::Legend::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

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

void cmp::Legend::lookAndFeelChanged() {
  if (auto* lnf =
          dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
