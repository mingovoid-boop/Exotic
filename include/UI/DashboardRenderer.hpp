#pragma once
#include <string>
#include "UI/ExoticUITheme.hpp"

namespace exotic {

class DashboardRenderer {
public:
    static std::string RenderTextPreview(const ExoticUITheme& theme);
};

}