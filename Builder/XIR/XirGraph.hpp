#pragma once
#include "XirNode.hpp"
#include "XirEdge.hpp"
#include <vector>
namespace exotic { struct XirGraph { std::vector<XirNode> nodes; std::vector<XirEdge> edges; void AddNode(const XirNode& node){ nodes.push_back(node); } void AddEdge(const XirEdge& edge){ edges.push_back(edge); } }; }
