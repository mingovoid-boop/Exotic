#include "exotic/cognition/metacognition.hpp"
#include <algorithm>
#include <cmath>
namespace exotic::cognition {
MetaChoice MetacognitiveCapability::consider(const std::string& subject,double uncertainty,double stakes)const{
  MetaChoice c; uncertainty=std::clamp(uncertainty,0.0,1.0);stakes=std::clamp(stakes,0.0,1.0);
  c.options={{"direct","Reason directly from current evidence",1.0-0.45*uncertainty,0.60},{"verify","Seek contradictions and verify assumptions",0.45+0.45*std::max(uncertainty,stakes),0.70},{"decompose","Break the subject into smaller independently testable questions",0.50+0.35*uncertainty,0.65},{"explore","Generate alternative hypotheses before committing",0.45+0.40*uncertainty,0.60},{"defer","Choose not to spend more cognition on this subject",0.30+0.45*(1.0-stakes),0.80}};
  // This is an offered capability, not a compulsory mental state. The executive may ignore it.
  c.engage=(uncertainty>=0.55||stakes>=0.70);
  c.reason=c.engage?"Metacognitive review is available because uncertainty or stakes are elevated; engagement remains optional.":"No strong reason to invoke metacognition; direct cognition remains available.";
  (void)subject;return c;
}
void MetacognitiveCapability::record_outcome(const std::string&strategy,double reward,const std::string&lesson){store_.append_episode({"meta-"+strategy+"-"+std::to_string(store_.episodes(128).size()+1),"Reasoning strategy "+strategy+": "+lesson,std::clamp(0.55+0.25*std::abs(reward),0.55,0.90),std::clamp(reward,-1.0,1.0),""});}
} // namespace exotic::cognition
