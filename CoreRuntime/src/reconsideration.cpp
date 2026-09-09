#include "exotic/cognition/reconsideration.hpp"
#include <algorithm>
namespace exotic::cognition {
ReconsiderationReview ReconsiderationEngine::consider(const Intention& current,const ReconsiderationSignal& raw,const std::optional<Intention>& alternative) const {
  const auto c=[](double v){return std::clamp(v,0.0,1.0);};
  const double evidence=c(raw.evidence_change), loss=c(raw.expected_loss), conflict=c(raw.conflict), gain=c(raw.opportunity_gain), inertia=c(raw.commitment_inertia);
  const double pressure=0.30*evidence+0.30*loss+0.20*conflict+0.20*gain;
  ReconsiderationReview r;
  r.review_worthwhile=pressure>=0.45;
  r.reason=r.review_worthwhile?"Changed evidence, conflict, expected loss, or opportunity makes reconsideration available; no reversal is required.":"Current signals do not strongly justify extra reconsideration; staying the course or doing nothing remain available.";
  const double stay=std::clamp(0.45+0.35*inertia-0.25*loss-0.20*evidence,0.0,1.0);
  const double suspend=std::clamp(0.20+0.30*conflict+0.25*evidence+0.15*loss-0.15*inertia,0.0,1.0);
  const double abandon=std::clamp(0.10+0.45*loss+0.25*evidence-0.30*inertia,0.0,1.0);
  r.options.push_back({ReconsiderationAction::StayCourse,current.id,"Continue the current intention despite new signals.",stay});
  r.options.push_back({ReconsiderationAction::Suspend,current.id,"Pause without abandoning the intention while more evidence is gathered.",suspend});
  if(alternative){const double sw=std::clamp(0.15+0.45*gain+0.20*conflict+0.15*evidence-0.20*inertia,0.0,1.0);r.options.push_back({ReconsiderationAction::Switch,alternative->id,"Switch attention to an alternative intention; this remains a voluntary option.",sw});}
  r.options.push_back({ReconsiderationAction::Abandon,current.id,"Abandon the current intention if the agent judges continuation no longer worthwhile.",abandon});
  r.options.push_back({ReconsiderationAction::DoNothing,"","Make no intention-state change during this review.",0.50});
  return r;
}
} // namespace exotic::cognition
