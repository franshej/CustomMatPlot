/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_legend.h"

#include "cmp_graph_line.h"
#include "cmp_plot.h"

void cmp::Legend::setLegend(const std::vector<std::string>& label_texts) {
  m_label_texts = label_texts;

  m_label_texts_is_changed = true;
}

void cmp::Legend::updateLegends(const GraphLines& graph_lines) {
  if ((m_legend_labels.size() != graph_lines.size() ||
       m_label_texts_is_changed)) {
    m_label_texts_is_changed = false;
    m_legend_labels.resize(m_label_texts.size());

    auto it_legend = m_legend_labels.begin();
    for (const auto& label : m_label_texts) {
      it_legend++->description = label;
    }

    m_legend_labels.resize(graph_lines.size());

    it_legend = m_legend_labels.begin();
    for (const auto& gl : graph_lines) {
      it_legend++->description_colour = gl->getColour();
    }

    if (onNumberOfDescriptionsChanged) {
      StringVector dislayed_descriptions(m_legend_labels.size());

      if (m_label_texts.size() >= m_legend_labels.size()) {
        std::copy(m_label_texts.begin(),
                  m_label_texts.begin() + m_legend_labels.size(),
                  dislayed_descriptions.begin());
      }

      onNumberOfDescriptionsChanged(dislayed_descriptions);
    }
  }
}

void cmp::Legend::resized() {}

void cmp::Legend::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

    lnf->drawLegend(g, m_legend_labels, getBounds());
  }
}

void cmp::Legend::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
