#pragma once
#include <string>
#include <vector>

namespace exotic {

struct UIButton {
    std::string id;
    std::string label;
    std::string icon;
};

struct ExoticUITheme {
    std::string name = "Exotic Remedy Enchanted Static;
 std::string background = black;
 std::string foreground = white;
 std::string accent = white-static;
 bool sharpBorders = true;
 bool enchantedSymbols = true;
 bool staticTexture = true;
 std::vector<UIButton> leftButtons = {
 {dashboard, DASHBOARD, pulse},
 {optimizer, OPTIMIZER, snowflake},
 {safety, SAFETY GATE, shield},
 {network, NETWORK, wave},
 {profiles, PROFILES, cube},
 {enchant, ENCHANT, star},
 {settings, SETTINGS, gear}
 };
 std::vector<UIButton> rightButtons = {
 {boost, BOOST, rocket},
 {cooling, COOLING, snowflake},
 {battery, BATTERY, battery},
 {network_boost, NETWORK BOOST, bars},
 {stability, STABILITY, shield},
 {enchant_mode, ENCHANT MODE, star},
 {terminal, TERMINAL, console}
 };
};

}
