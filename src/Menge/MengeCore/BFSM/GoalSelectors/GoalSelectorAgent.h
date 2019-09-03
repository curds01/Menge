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
 @file    GoalSelectorAgent.h
 @brief   The definition of the agent goal selector.
 */

#pragma once

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorSet.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/CoreConfig.h"

namespace Menge {
namespace BFSM {

/*!
 @brief   A goal selector that allows an agent to pick *another* agent to define its goal position;
          in essence, it causes one agent to follow another.

 <h3>XML Specification</h3>

 To specify an agent goal selector, use the following syntax:

 @code{xml}
 <State ...>
   <VelComponent .../>
   <GoalSelector type="agent" 
                 criteria={"nearest"|"farthest"|"random"|"nearest_nav_mesh"|"farthest_nav_mesh"}
                 nav_mesh="file_name.nav" >
     <Target type="named_state_member" name="agents_in_walk" is_member="1" state="Walk"/>
   </GoalSelector>
 </State>
 @endcode

 The agent goal selector has parameters *and* supports further specification via child tags.

 The parameters:

   - `criteria`: determines the mechanism for selecting one agent from a set.
     - "nearest": the nearest agent based on Euclidian distance in Cartesian space.
     - "farthest": the farthest agent based on Euclidian distance in Caretsian space.
     - "random": an agent is selected with uniform probability from the set.
     - "nearest_nav_mesh": the agent with the shortest path as defined by the indicated nav mesh.
     - "farthest_nav_mesh": the agent with the longest path as defined by the indicated nav mesh.
  - `nav_mesh`: the name of the navigation mesh to load (only required if criteria requires a
                navigation mesh.

 Child tags:

 If no child tags are given, the goal selector will draw from all agents in the simulator (except
 the agent being assigned the goal). Otherwise, the tags can be used to specify a subset of
 the population to pick one from.

 We re-use the AgentEventTarget functionality for specifing the set of agent. Each implementation
 of AgentEventTarget returns a set of agents according to a particular criteria. A sub-tree of
 such tags can be constructed to indicate the subset of agents to draw from. For example:

 @code{xml}
 <Target type="agent_union">
   <Target type="named_state_member" name="agents_in_walk1" is_member="1" state="Walk1"/>
   <Target type="named_state_member" name="agents_in_walk2" is_member="1" state="Walk2"/>
 </Target>
 @endcode

 provided as a child of the agent goal selector, will only consider the agents in the "Walk1" state
 or the "Walk2" states; agents in all other states are excluded.
 */
class MENGE_API AgentGoalSelector final : public GoalSelector{
 public:

};

}  // namespace BFSM
} // namespace Menge
