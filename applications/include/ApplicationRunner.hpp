#pragma once

#include "Application.hpp"

namespace exotic {
class ApplicationRunner {
public:
    int Run(Application& app) {
        ExoticContext context;
        if (!app.Initialize(context)) return -1;
        app.Update(context);
        app.Shutdown(context);
        return 0;
    }
};
}

