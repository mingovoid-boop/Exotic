#pragma once
#include <string>
namespace exotic { class TemplateEngine { public: static std::string Header(const std::string& name); static std::string Source(const std::string& name); }; }
