#include "cmp_downsampler.h"
#include "cmp_test_helper.hpp"
#include <vector>

class DownsamplerTest : public juce::UnitTest {
public:
    DownsamplerTest() : juce::UnitTest("DownsamplerTest", "Downsampler") {}
    
    void runTest() override {
        TEST("Empty data") {
            std::vector<float> x_data;
            std::vector<std::size_t> indices;
            cmp::Downsampler<float>::calculateXIndices(
                cmp::Scaling::linear,
                {0.f, 1.f},
                juce::Rectangle<int>(0, 0, 100, 100),
                x_data,
                indices
            );
            expect(indices.empty());
        }

        TEST("XY downsampling with spikes") {
            // Create data with a spike that should be preserved
            std::vector<float> x_data(1000, 0.0f);
            std::vector<float> y_data(1000, 0.0f);
            
            // Add a spike in the middle
            const size_t spike_index = 500;
            y_data[spike_index] = 100.0f;
            
            std::vector<std::size_t> x_indices;
            std::vector<std::size_t> xy_indices;
            
            cmp::Downsampler<float>::calculateXIndices(
                cmp::Scaling::linear,
                {0.f, 1000.f},
                juce::Rectangle<int>(0, 0, 100, 100),
                x_data,
                x_indices
            );
            
            cmp::Downsampler<float>::calculateXYBasedIdxs(
                x_indices,
                y_data,
                xy_indices
            );
            
            // The spike should be preserved in downsampling
            expect(std::find(xy_indices.begin(), xy_indices.end(), spike_index) != xy_indices.end());
        }

        TEST("XY downsampling with alternating values") {
            std::vector<float> x_data(100);
            std::vector<float> y_data(100);
            std::iota(x_data.begin(), x_data.end(), 0.f);
            
            // Create alternating high/low values
            for(size_t i = 0; i < y_data.size(); ++i) {
                y_data[i] = (i % 2) ? 100.f : 0.f;
            }
            
            std::vector<std::size_t> x_indices;
            std::vector<std::size_t> xy_indices;
            
            cmp::Downsampler<float>::calculateXIndices(
                cmp::Scaling::linear,
                {0.f, 100.f},
                juce::Rectangle<int>(0, 0, 100, 100),
                x_data,
                x_indices
            );
            
            cmp::Downsampler<float>::calculateXYBasedIdxs(
                x_indices,
                y_data,
                xy_indices
            );
            
            // Should preserve enough points to show the alternating pattern
            expectGreaterThan(xy_indices.size(), x_indices.size());
        }

        TEST("XY downsampling with constant segments") {
            std::vector<float> x_data = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f};
            std::vector<float> y_data = {0.f, 0.f, 1.f, 1.f, 0.f, 0.f};
            
            std::vector<std::size_t> x_indices;
            std::vector<std::size_t> xy_indices;
            
            cmp::Downsampler<float>::calculateXIndices(
                cmp::Scaling::linear,
                {0.f, 5.f},
                juce::Rectangle<int>(0, 0, 100, 100),
                x_data,
                x_indices
            );
            
            cmp::Downsampler<float>::calculateXYBasedIdxs(
                x_indices,
                y_data,
                xy_indices
            );
            
            // Should preserve transition points
            expect(std::find(xy_indices.begin(), xy_indices.end(), 2) != xy_indices.end());
            expect(std::find(xy_indices.begin(), xy_indices.end(), 3) != xy_indices.end());
        }

        TEST("XY downsampling with edge transitions") {
            std::vector<float> x_data(200);
            std::vector<float> y_data(200, 0.0f);
            std::iota(x_data.begin(), x_data.end(), 0.f);
            
            // Create a signal with important transitions at edges
            y_data[0] = 1.0f;   // Start high
            y_data[1] = 0.0f;   // Immediate drop
            y_data[198] = 0.0f; // End low
            y_data[199] = 1.0f; // Final spike
            
            std::vector<std::size_t> x_indices;
            std::vector<std::size_t> xy_indices;
            
            cmp::Downsampler<float>::calculateXIndices(
                cmp::Scaling::linear,
                {0.f, 200.f},
                juce::Rectangle<int>(0, 0, 100, 100),
                x_data,
                x_indices
            );
            
            cmp::Downsampler<float>::calculateXYBasedIdxs(
                x_indices,
                y_data,
                xy_indices
            );
            
            // Should preserve both start and end transitions
            expect(std::find(xy_indices.begin(), xy_indices.end(), 0) != xy_indices.end());
            expect(std::find(xy_indices.begin(), xy_indices.end(), 1) != xy_indices.end());
            expect(std::find(xy_indices.begin(), xy_indices.end(), 198) != xy_indices.end());
            expect(std::find(xy_indices.begin(), xy_indices.end(), 199) != xy_indices.end());
        }
    }
};

static DownsamplerTest downsamplerTest; 