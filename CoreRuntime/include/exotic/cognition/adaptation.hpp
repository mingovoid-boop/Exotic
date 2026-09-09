#pragma once
#include <string>
#include <vector>
namespace exotic::cognition {
enum class AdaptationTarget { Belief, Preference, Drive, Strategy, Goal };
enum class AdaptationDecision { Adopt, Defer, Reject, ObserveMore };
struct AdaptationEvidence { std::string id; std::string description; double valence{0.0}; double reliability{0.5}; double relevance{0.5}; };
struct AdaptationProposal { std::string id; AdaptationTarget target{AdaptationTarget::Belief}; std::string target_id; double suggested_delta{0.0}; double confidence{0.0}; std::string rationale; };
struct AdaptationChoice { AdaptationDecision decision{AdaptationDecision::ObserveMore}; AdaptationProposal proposal; std::string rationale; };
class AdaptationCapability {
 public:
  [[nodiscard]] std::vector<AdaptationProposal> propose(const std::vector<AdaptationEvidence>& evidence,AdaptationTarget target,const std::string& target_id) const;
  [[nodiscard]] AdaptationChoice consider(const AdaptationProposal& proposal) const;
};
} // namespace exotic::cognition
