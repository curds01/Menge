/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 @file    GoalAgent.h
 @brief   The definition of a BFSM goal that is fixed to another agent.
 */

#pragma once

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/CoreConfig.h"

namespace Menge {
namespace BFSM {

/*!
 @brief    A goal that stays fixed to an agent.

 The %AgentGoal is a circular goal that is unique in that its size and position are defined by
 an agent in the simulation. When the goal is created, it is associated with a *target* agent (*not*
 the agent who is trying to reach the goal). The radius of the goal is that of the target agent and
 the center of the goal is always at the target agent's center. This goal allows an agent to 
 follow another agent.
 
 If the agent becomes invalid, the goal reverts to behaving like an "identity" goal -- in that its
 position becomes that of the agent trying to reach the goal. This will make any "goal reached"
 transition instantly satisfied.

 @note Agents don't yet become invalid. In the future, when sources and sinks are introduced, agents
 can be removed from the simulation. At that point, any %AgentGoal referencing that agent will 
 become the identity.

 <h3>XML specification</h3>

 %AgentGoals are not explicitly instantiated in XML. Instead, the AgentGoalSelector is used to
 create instances of %AgentGoal as necessary -- it is also responsible for providing the logic that
 defines which agent gets picked as the goal target.
 */
class MENGE_API AgentGoal final : public Goal {
 public:
  AgentGoal(const Agents::BaseAgent& target_agent, const Agents::BaseAgent& follow_agent);

  // Inherits docs from Goal::getStringId().
  std::string getStringId() const final { return NAME; }

  // Inherits docs from Goal::moves().
  bool moves() const final { return true; }

  // Inherits docs from Goal::move().
  void move(float time_step) final;

  /*!
   @brief   Clears the target agent -- behavior reverts to "identity"-like goal on follower agent.
   */
  void clearTarget() { _target_agent = nullptr; }

    /*! The unique identifier used to register this type with run-time components. */
  static const std::string NAME;

 private:
  // The agent serving as the target of the goal.
  const Agents::BaseAgent* _target_agent{nullptr};

  // The agent following the target -- when there is no target agent, the goal position comes from
  // this agent.
  const Agents::BaseAgent* _follow_agent{nullptr};
};

}  // namespace BFSM
}  // namespace Menge
