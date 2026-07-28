#pragma once
#include "XirGraph.hpp"
namespace exotic { class XirValidator { public: static bool Validate(const XirGraph& graph){ return !graph.nodes.empty(); } }; }
