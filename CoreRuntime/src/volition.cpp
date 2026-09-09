#include "exotic/cognition/volition.hpp"
#include <algorithm>
#include <cmath>
namespace exotic::cognition {
double VolitionEngine::support(const VolitionOption&o){return 0.18*o.desire+0.20*o.commitment+0.14*o.curiosity+0.16*o.user_relevance+0.22*o.expected_value-0.05*o.cost-0.05*o.risk;}
VolitionDecision VolitionEngine::decide(const std::vector<VolitionOption>&options)const{
  if(options.empty()) return {VolitionOutcome::DoNothing,"","No available option warrants a choice.",0.0};
  const auto best=std::max_element(options.begin(),options.end(),[](const auto&a,const auto&b){return support(a)<support(b);});
  const double s=support(*best);
  if(s<0.20) return {VolitionOutcome::DoNothing,"","Available signals are too weak to justify commitment.",s};
  if(best->risk>=0.85&&best->expected_value<0.70) return {VolitionOutcome::Reject,best->id,"Risk dominates expected value; reject this option without imposing an alternate state.",s};
  if(best->cost>=0.80&&best->expected_value<0.75) return {VolitionOutcome::Defer,best->id,"The option remains available, but current cost argues for deferral.",s};
  return {VolitionOutcome::Choose,best->id,"Selected after weighing competing signals; no individual signal was treated as compulsory.",s};
}
} // namespace exotic::cognition
