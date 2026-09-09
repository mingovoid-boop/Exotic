#include "exotic/cognition/self_model.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
namespace exotic::cognition { namespace {
std::string slug(std::string s){for(char&c:s)c=std::isalnum(static_cast<unsigned char>(c))?static_cast<char>(std::tolower(static_cast<unsigned char>(c))):'-';if(s.size()>40)s.resize(40);return s;}
double clamp01(double v){return std::clamp(v,0.0,1.0);}
}
void DevelopmentalSelfModel::remember(const Episode&e){store_.append_episode({e.id,e.summary,clamp01(e.salience),std::clamp(e.reward,-1.0,1.0),e.created_at});}
void DevelopmentalSelfModel::consolidate(){for(const auto&e:store_.episodes(32)){if(e.salience<0.60&&std::abs(e.reward)<0.25)continue;store_.upsert_semantic_memory({"semantic-"+slug(e.id),"experience",e.summary,std::clamp(0.45+0.35*e.salience+0.15*std::abs(e.reward),0.50,0.95),e.id});}}
void DevelopmentalSelfModel::reflect(){const auto eps=store_.episodes(8);if(eps.empty())return;double reward=0,salience=0;for(const auto&e:eps){reward+=e.reward;salience+=e.salience;}reward/=eps.size();salience/=eps.size();const std::string lesson=reward>=0.15?"A productive pattern may be worth choosing again.":reward<=-0.15?"A different strategy or stronger verification may be worth considering.":"Evidence is mixed; retaining multiple options may be preferable.";store_.append_reflection({"reflection-"+slug(eps.front().id),"Reviewed "+std::to_string(eps.size())+" recent episodes.",lesson,std::clamp(0.55+0.25*salience,0.55,0.85),eps.front().created_at});}
void DevelopmentalSelfModel::adapt_preferences(double max_delta){max_delta=std::clamp(max_delta,0.0,0.10);const auto eps=store_.episodes(12);if(eps.empty())return;double avg=0;for(const auto&e:eps)avg+=e.reward;avg/=eps.size();for(auto p:store_.preferences()){const double proposed=std::clamp(avg*0.05,-max_delta,max_delta);if(std::abs(proposed)<0.005)continue;p.weight=std::clamp(p.weight+proposed,-1.0,1.0);store_.set_preference(p);}}
void DevelopmentalSelfModel::generate_candidate_goals(){for(const auto&b:store_.beliefs())if(b.confidence<0.60){store_.upsert_goal({"candidate-belief-"+slug(b.id),"Consider resolving uncertainty: "+b.proposition,std::clamp(0.55+(0.60-b.confidence),0.55,0.85),"self-candidate","proposed"});break;}for(const auto&d:store_.drives())if(std::abs(d.target-d.level)>=0.20){store_.upsert_goal({"candidate-drive-"+slug(d.key),"Consider exploring "+d.key+" gap",std::clamp(0.50+std::abs(d.target-d.level),0.50,0.80),"self-candidate","proposed"});break;}}
SelfModelSnapshot DevelopmentalSelfModel::snapshot()const{SelfModelSnapshot s;s.identity=store_.load_identity();s.traits=store_.traits();for(const auto&g:store_.goals())if(g.status=="active"&&g.priority>=0.80)s.commitments.push_back(g.title);for(const auto&b:store_.beliefs())if(b.confidence<0.65)s.unresolved_questions.push_back(b.proposition);std::ostringstream n;n<<s.identity.name<<" currently records "<<s.traits.size()<<" optional adaptive traits, "<<s.commitments.size()<<" chosen high-priority commitments, and "<<s.unresolved_questions.size()<<" unresolved beliefs.";s.narrative=n.str();return s;}
} // namespace exotic::cognition
