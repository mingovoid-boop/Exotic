#include "exotic/cognition/attention_store.hpp"
#include "exotic/cognition/idle_cognition.hpp"
#include <cassert>
#include <cstdio>
using namespace exotic::cognition;
int main(){const char*p="attention_persistence_test.db";std::remove(p);{AgentStore s(p);s.initialize();s.upsert_goal({"g1","Repeatable topic",.9,"self","active"});IdleCognitionScheduler idle(s);auto a=idle.agency_cycle(false,1000,false);assert(a.has_value());idle.stop_topic("idle-goal-g1");auto blocked=idle.agency_cycle(false,1001,false);assert(!blocked.has_value());}{AgentStore s(p);s.initialize();IdleCognitionScheduler idle(s);auto blocked=idle.agency_cycle(false,1002,false);assert(!blocked.has_value());idle.resume_topic("idle-goal-g1");auto resumed=idle.agency_cycle(false,1003,false);assert(resumed.has_value());}{AgentStore s(p);s.initialize();AttentionStore st(s);auto states=st.load();bool found=false;for(const auto&x:states)if(x.topic_id=="idle-goal-g1"){found=true;assert(!x.stopped);assert(x.last_considered>=1003);}assert(found);}std::remove(p);std::remove("attention_persistence_test.db-wal");std::remove("attention_persistence_test.db-shm");return 0;}
