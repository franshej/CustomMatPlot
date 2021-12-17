#include "scp_trace.h"

#include "spl_plot.h"

void scp::Trace::addOrRemoveTracePoint(const juce::Point<float>& trace_point) {
  if (std::find_if(m_trace_points.begin(), m_trace_points.end(),
                   [&trace_point](const auto& tp) {
                     return (*tp) == trace_point;
                   }) == m_trace_points.end()) {
    m_trace_points.emplace_back(std::make_unique<TracePoint_f>());
  } else {
    std::remove_if(
        m_trace_points.begin(), m_trace_points.end(),
        [&trace_point](const auto& tp) { return (*tp) == trace_point; });
  }
}

void scp::Trace::resized() {}

void scp::Trace::paint(juce::Graphics& g) {}

void scp::Trace::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
