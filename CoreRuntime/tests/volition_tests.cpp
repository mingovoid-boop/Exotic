#include "exotic/cognition/volition.hpp"
#include <cassert>
using namespace exotic::cognition;
int main(){VolitionEngine v;auto none=v.decide({});assert(none.outcome==VolitionOutcome::DoNothing);auto weak=v.decide({{"a","weak",0.1,0.1,0.1,0.1,0.1,0.1,0.1}});assert(weak.outcome==VolitionOutcome::DoNothing);auto risky=v.decide({{"r","risky",0.9,0.8,0.6,0.4,0.4,0.1,0.95}});assert(risky.outcome==VolitionOutcome::Reject);auto costly=v.decide({{"c","costly",0.8,0.8,0.5,0.5,0.6,0.9,0.2}});assert(costly.outcome==VolitionOutcome::Defer);auto chosen=v.decide({{"x","chosen",0.7,0.7,0.6,0.6,0.9,0.2,0.1},{"y","alternate",0.4,0.3,0.8,0.2,0.4,0.2,0.1}});assert(chosen.outcome==VolitionOutcome::Choose);assert(chosen.option_id=="x");return 0;}
