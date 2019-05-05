/*

License

Menge
Copyright © and trademark ™ 2019 Sean Curtis
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include <iostream>
#include <utility>

#include "MengeCore/Agents/AgentInitializer.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQueryKDTree.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorExplicit.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorIdentity.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/GoalPoint.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/Transitions/CondGoal.h"
#include "MengeCore/BFSM/Transitions/Target.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/VelocityComponents/VelCompGoal.h"
#include "MengeCore/Math/Vector2.h"
#include "MengeCore/orca/ORCASimulator.h"

int main(int argc, char* argv[]) {
  ORCA::Simulator simulator;

  // NOTE: I can't set the values in the initializer.
  Menge::Agents::AgentInitializer initializer;
  // The only interface for adding an agent is this: position + agent initializer.
  simulator.addAgent(Menge::Math::Vector2(2, 0), &initializer);
  simulator.addAgent(Menge::Math::Vector2(-2, 0.001), &initializer);

  simulator.setSpatialQuery(new Menge::Agents::BergKDTree());
  simulator.initSpatialQuery();

  // Building the simple FSM. The simulator will eventually take ownership of this.
  Menge::BFSM::FSM* fsm = new Menge::BFSM::FSM(&simulator);

  // Add goals
  Menge::BFSM::GoalSet* goal_set = new Menge::BFSM::GoalSet();
  fsm->getGoalSets()[0] = goal_set;  // FSM takes ownership.
  goal_set->addGoal(0, new Menge::BFSM::PointGoal(Menge::Math::Vector2(-3, 0)));
  goal_set->addGoal(1, new Menge::BFSM::PointGoal(Menge::Math::Vector2(3, 0)));

  // Add states
  std::map<std::string, Menge::BFSM::State*> state_map;
  auto make_state = [&state_map, &fsm](std::string name, bool is_final, int goal_id) {
    // NOTE: State needs MENGE_API applied to it so it can be constructed here.
    Menge::BFSM::State* state = new Menge::BFSM::State(std::move(name));
    state->setFinal(is_final);
    if (is_final) {
      state->setGoalSelector(new Menge::BFSM::IdentityGoalSelector());
    } else {
      Menge::BFSM::ExplicitGoalSelector* goal_selector = new Menge::BFSM::ExplicitGoalSelector();
      goal_selector->setGoalSetID(0);
      goal_selector->setGoalID(goal_id);
      goal_selector->setGoalSet(fsm->getGoalSets());
      state->setGoalSelector(goal_selector);
    }
    state->setVelComponent(new Menge::BFSM::GoalVelComponent());
    state_map[state->getName()] = state;
    fsm->addNode(state);
    return state;
  };
  Menge::BFSM::State* walk_1 = make_state("Walk1", false, 0);
  Menge::BFSM::State* stop_1 = make_state("Stop1", true, 0);
  Menge::BFSM::State* walk_2 = make_state("Walk2", false, 1);
  Menge::BFSM::State* stop_2 = make_state("Stop2", true, 1);

  // Add transitions.
  auto make_transition = [&state_map, &fsm](const std::string& from_name,
                                            const std::string& to_name) {
    Menge::BFSM::GoalCondition* cond1 = new Menge::BFSM::GoalCondition();
    cond1->setMinDistance(0.2f);
    Menge::BFSM::SingleTarget* target1 = new Menge::BFSM::SingleTarget(to_name);
    // NOTE: Transition needs MENGE_API applied to it so it can be constructed here.
    Menge::BFSM::Transition* trans1 = new Menge::BFSM::Transition(cond1, target1);
    trans1->connectStates(state_map);
    Menge::BFSM::State* state = state_map.at(from_name);
    fsm->addTransition(state->getID(), trans1);
  };
  make_transition("Walk1", "Stop1");
  make_transition("Walk2", "Stop2");

  // Initialize all agents.
  auto init_agent_state = [&simulator, &fsm](int i, Menge::BFSM::State* state) {
    Menge::Agents::BaseAgent* agent = simulator.getAgent(i);
    fsm->setCurrentState(agent, state->getID());
    state->enter(agent);
  };
  init_agent_state(0, walk_1);
  init_agent_state(1, walk_2);

  // Finalize.
  simulator.setBFSM(fsm);
  const int agent_count = static_cast<int>(simulator.getNumAgents());
  for (int i = 0; i < agent_count; ++i) {
    fsm->computePrefVelocity(simulator.getAgent(i));
  }
  simulator.finalize();
  fsm->finalize();

  simulator.setTimeStep(0.1f);
  simulator.setMaxDuration(5.f);

  bool running = true;
  do {
    running = simulator.step();
    std::cout << "Time: " << simulator.getGlobalTime() << "\n";
    std::cout << "   Agent 0: " << simulator.getAgent(0)->_pos << "\n";
    std::cout << "   Agent 1: " << simulator.getAgent(1)->_pos << "\n";
  } while (running);

  return 0;
}
