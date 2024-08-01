#pragma once

#include <juce_core/juce_core.h>
#include "cmp_plot.h"

template <typename ComponentType> 
static std::vector<ComponentType*> getChildComponentHelper(cmp::Plot& parent_component){
    std::vector<ComponentType*> child_components;

    for (int i = 0; i < parent_component.getNumChildComponents(); i++) {
        auto* child_component = parent_component.getChildComponent(i);
        if (auto* child = dynamic_cast<ComponentType*>(child_component)) {
            child_components.push_back(child);
        }
    }

    return child_components;
}

static void expectEqualVectors(const std::vector<float>& v1, const std::vector<float>& v2, const auto expectEquals){
    expectEquals(v1.size(), v2.size());
    for (int i = 0; i < v1.size(); i++){
        expectEquals(v1[i], v2[i]);
    }
}