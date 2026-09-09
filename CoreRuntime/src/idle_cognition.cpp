#include "exotic/cognition/idle_cognition.hpp"
#include "exotic/cognition/self_model.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>

namespace exotic::cognition {

bool IdleCognitionScheduler::should_run(bool externally_busy) const {
  if (externally_busy) return false;
  const auto drives = store_.drives();
  return std::any_of(drives.begin(), drives.end(), [](const DriveRecord& d) {
    return std::abs(d.target - d.level) >= 0.10;
  }) || !store_.goals().empty() || !store_.beliefs().empty();
}

std::optional<ThoughtCandidate> IdleCognitionScheduler::next_candidate() const {
  DevelopmentalSelfModel self(store_);
  self.consolidate();
  self.reflect();
  self.adapt_preferences(0.03);
  self.generate_candidate_goals();

  const auto goals = store_.goals();
  const auto beliefs = store_.beliefs();
  const auto preferences = store_.preferences();

  const auto active = std::find_if(goals.begin(), goals.end(), [](const PersistentGoal& g) {
    return g.status == "active";
  });
  if (active != goals.end()) {
    return ThoughtCandidate{
        "idle-goal-" + active->id,
        "Advance goal: " + active->title,
        active->priority,
        0.45,
        0.65,
        0.35,
        0.05,
        0.40,
        0.05};
  }

  const auto uncertain = std::find_if(beliefs.rbegin(), beliefs.rend(), [](const BeliefRecord& b) {
    return b.confidence < 0.70;
  });
  if (uncertain != beliefs.rend()) {
    return ThoughtCandidate{
        "idle-belief-" + uncertain->id,
        "Re-evaluate belief: " + uncertain->proposition,
        0.55,
        1.0 - uncertain->confidence,
        0.75,
        0.30,
        0.10,
        0.55,
        0.05};
  }

  if (!preferences.empty()) {
    const auto strongest = std::max_element(preferences.begin(), preferences.end(), [](const auto& a, const auto& b) {
      return std::abs(a.weight) < std::abs(b.weight);
    });
    return ThoughtCandidate{
        "idle-preference-" + strongest->key,
        "Reflect on preference: " + strongest->key,
        std::min(1.0, std::abs(strongest->weight)),
        0.35,
        0.45,
        0.55,
        0.05,
        0.50,
        0.05};
  }

  return std::nullopt;
}

void IdleCognitionScheduler::apply_reward(double reward) {
  for (auto drive : store_.drives()) {
    const double delta = drive.learning_rate * reward;
    drive.level = std::clamp(drive.level + delta, 0.0, 1.0);
    store_.set_drive(drive);
  }

  const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
  DevelopmentalSelfModel self(store_);
  self.remember({
      "reward-episode-" + std::to_string(tick),
      reward >= 0.0 ? "Operator feedback reinforced the recent cognitive pattern." : "Operator feedback discouraged the recent cognitive pattern.",
      std::clamp(0.55 + std::abs(reward) * 0.35, 0.0, 1.0),
      std::clamp(reward, -1.0, 1.0),
      ""});
}

}  // namespace exotic::cognition
