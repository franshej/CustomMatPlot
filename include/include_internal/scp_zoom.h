/**
 * @file scp_frame.h
 *
 * @brief Componenet for creating a frame
 *
 * @ingroup SimpleCustomPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <scp_datamodels.h>

namespace scp {
	class Zoom : public juce::Component {
	public:
		void resized() override;
		void paint(juce::Graphics& g) override;
		void lookAndFeelChanged() override;

	private:
		LookAndFeelMethodsBase* m_lookandfeel;
	};
}



