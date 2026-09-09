#include "exotic/cognition/arbitration.hpp"
#include <algorithm>
#include <unordered_set>
namespace exotic::cognition { namespace {
bool has(const std::vector<std::string>&v,const std::string&id){return std::find(v.begin(),v.end(),id)!=v.end();}
double merit(const IntentionDemand&d){return std::clamp(0.35*d.intention.strength+0.30*d.urgency+0.35*d.expected_value-0.20*d.attention_cost-0.15*d.resource_cost,0.0,1.0);}
}
ArbitrationPlan MultiIntentionArbiter::consider(const std::vector<IntentionDemand>&demands,double capacity)const{
 ArbitrationPlan p; capacity=std::clamp(capacity,0.0,1.0);p.unused_capacity=capacity;
 std::vector<const IntentionDemand*> active;for(const auto&d:demands){if(d.intention.state=="committed"||d.intention.state=="tentative")active.push_back(&d);else p.assignments.push_back({d.intention.id,ArbitrationDisposition::Unscheduled,0.0,"Inactive intention remains available but is not scheduled."});}
 std::stable_sort(active.begin(),active.end(),[](auto*a,auto*b){return merit(*a)>merit(*b);});std::unordered_set<std::string> running;
 for(const auto*d:active){bool conflict=false,coop=false;for(const auto&id:running){conflict=conflict||has(d->conflicts_with,id);coop=coop||has(d->cooperates_with,id);}if(conflict){p.assignments.push_back({d->intention.id,ArbitrationDisposition::Blocked,0.0,"Conflicts with a currently allocated intention; remains intact for later reconsideration."});continue;}const double wanted=std::clamp(0.15+0.55*merit(*d),0.0,1.0);if(p.unused_capacity<=0.05){p.assignments.push_back({d->intention.id,ArbitrationDisposition::Yield,0.0,"Chooses to yield this scheduling window; intention is not abandoned."});continue;}const double give=std::min(wanted,p.unused_capacity);p.unused_capacity-=give;running.insert(d->intention.id);p.assignments.push_back({d->intention.id,coop?ArbitrationDisposition::Cooperate:ArbitrationDisposition::Run,give,coop?"Shares capacity with a compatible intention.":"Receives temporary capacity without becoming a permanent primary goal."});}
 return p;
}
} // namespace exotic::cognition
