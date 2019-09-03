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
 @file    AgentEventTargetSetOp.h
 @brief   Definition of several event targets that operate on sets of agents. It provides the basic
          set operations so that indepenent criteria can be combined in a meaningful way.
 */

#pragma once

#include <vector>

#include "MengeCore/Agents/Events/AgentEventTarget.h"
#include "MengeCore/Agents/Events/EventTarget.h"
#include "MengeCore/Agents/Events/EventTargetFactory.h"

namespace Menge {
/*!
 @brief    The target for an agent-target event which provides the union of two or more
           AgentEventTarget impliementations.

 %AgentSetUnionTarget defines the set union on sets of agents. The target of an event can be defined
 by combining the results of several otherwise independent agent target implementations.

 <h3>XML specification</h3>

 To specify an agent set union, use the following syntax:

 @code{xml}
 <EventSystem conservative="1">
   <Target type="agent_union" name="some_name">
     <Target name="subset1" type="[agent target type]" ... />
     <Target name="subset2" type="[agent target type]" ... />
     ...
     <Target name="subsetN" type="[agent target type]" ... />
   </Target>
   <Event name="some_event">
     <Triger ... />
     <Response effect="makeFatter" target="some_name" />
   </Event>
 </EventSystem>
 @endcode

 The `agent_union` type has no unique parameters. However, it does have one or more child tags.
 Each child tag must, in turn, be an implementaiton of an AgentEventTarget. The order is irrelevant
 as the resulting set of agents will be the combination of all unique agents. Omitting child targets
 is not an error, but will emit a warning.

 <h3>Union vs multi-Response</h3>

 As with much in Menge, circumstances in which this construct would be used may be articulated
 in a different, but functionally equivalent way. For example:

 @code{xml}
 <EventSystem conservative="1">
   <Effect name="some_effect" ... />
   <Target type="agent_union" name="union_name">
     <Target name="set1" type="some_agent_target" ... />
     <Target name="set2" type="some_agent_target" ... />
   </Target>
   <Event name="unioned_event">
     <Triger ... />
     <Response effect="some_effect" target="union_name" />
   </Event>
 </EventSystem>
 @endcode
 __%Event which acts on the *union* of the agents in two sets.__


 @code{xml}
 <EventSystem conservative="1">
   <Effect name="some_effect" ... />
   <Target name="set1" type="some_agent_target" ... />
   <Target name="set2" type="some_agent_target" ... />
   <Event name="independent_event">
     <Triger ... />
     <Response effect="some_effect" target="set1" />
     <Response effect="some_effect" target="set2" />
   </Event>
 </EventSystem>
 @endcode
 __%Event which acts on the agents in two sets, independently.__

 In these two examples, we assume that the definitions of the trigger and the effect are identical.
 They only vary in how they define the target of the effect. In the first example, we create the
 union of two sets of agents and declare a *single* response. In the second, we define the identical
 two sets of agents. But we define two responses (using the same effect) -- each response affecting
 one of the two sets.

 So, are these two events equivalent? The answer is, "maybe" or "it depends". It is impossible to
 know without knowing something about the sets and/or the effect.

   - If the events are truly disjoint (i.e., no common agents), then, regardless of the effect,
     these two events have the same effect. One way to guarantee two disjoint set of agents is to
     define each set as the agents in a single state -- as every agent belongs to one and only one
     state at any given time.
   - If the sets of agents have agents in common, then it depends on the effect. In the unioned set
     of agents, we guarantee that the effect will be applied to each agent only once. In the
     multi-response event, each agent that appears in both sets will have the effect applied
     *twice*.
       - If the effect is idempotent, then the two events are the same. An example of an idempotent
         effect would be to move a target agent to a specific state. Even if I redundantly take that
         action, the agent will still end up in the same state.
       - If the effect has an accumulative influence (like offsetting an agent parameter), then the
         agents which appear in both sets will have two offsets applied.
    - Events are processed in the order declared. Similarly, in a single event, each response is
      processed in the order declared. Processing a response entails collecting target agents and
      applying the effect to the target. That means, the first response could end up changing the
      members of subsequent responses. In the case of the union, we guarantee that all constituent
      targets are evaluated while the simulator is in a fixed configuration and all will get the
      effect applied at once.
 */
class MENGE_API AgentSetUnionTarget final : public AgentEventTarget {
 public:
  ~AgentSetUnionTarget();

  // Inherits docs from EventTarget::finalize().
  void finalize() final;

 protected:
  friend class AgentSetUnionTargetFactory;

  // Inherits docs from EventTarget::finalize().
  void doUpdate() final;

  /*!
   @brief    The set of agent targets to combine.
   */
  std::vector<AgentEventTarget*> _agent_targets;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    Factory for the AgentSetUnionTarget.
 */
class MENGE_API AgentSetUnionTargetFactory final : public AgentEventTargetFactory {
 public:
  // Inherits docs from AgentEventTargetFactory::name().
  const char* name() const final { return "agent_union"; }

  // Inherits docs from AgentEventTargetFactory::description().
  const char* description() const final {
    return "Defines a set of agents as the target by unioning together one or more child agent "
           "targets.";
  }

 protected:
  // Inherits docs from AgentEventTargetFactory::instance().
  EventTarget* instance() const final { return new AgentSetUnionTarget(); }

  // Inherits docs from AgentEventTargetFactory::setFromXML().
  bool setFromXML(EventTarget* target, TiXmlElement* node,
                  const std::string& behaveFldr) const final;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    The target for an agent-target event which provides the intersection of two or more
           AgentEventTarget impliementations.

 %AgentSetIntersectionTarget defines the set intersection of sets of agents. The allows the target
 of an event to be defined multiple otherwise independent criteria.

 <h3>XML specification</h3>

 To specify an agent set intersection, use the following syntax:

 @code{xml}
 <EventSystem conservative="1">
   <Target type="agent_intersect" name="some_name">
     <Target name="subset1" type="[agent target type]" ... />
     <Target name="subset2" type="[agent target type]" ... />
     ...
     <Target name="subsetN" type="[agent target type]" ... />
   </Target>
   <Event name="some_event">
     <Triger ... />
     <Response effect="makeFatter" target="some_name" />
   </Event>
 </EventSystem>
 @endcode

 The `agent_intersect` type has no unique parameters. However, it does have one or more child tags.
 Each child tag must, in turn, be an implementaiton of an AgentEventTarget. The order is irrelevant
 as the resulting set of agents will be the agents present in all child sets. Omitting child targets
 is not an error, but will emit a warning.
 */
class MENGE_API AgentSetIntersectionTarget final : public AgentEventTarget {
 public:
  ~AgentSetIntersectionTarget();

  // Inherits docs from EventTarget::finalize().
  void finalize() final;

 protected:
  friend class AgentSetIntersectionTargetFactory;

  // Inherits docs from EventTarget::finalize().
  void doUpdate() final;

  /*!
   @brief    The set of agent targets to combine.
   */
  std::vector<AgentEventTarget*> _agent_targets;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    Factory for the AgentSetIntersectionTarget.
 */
class MENGE_API AgentSetIntersectionTargetFactory final : public AgentEventTargetFactory {
 public:
  // Inherits docs from AgentEventTargetFactory::name().
  const char* name() const final { return "agent_intersect"; }

  // Inherits docs from AgentEventTargetFactory::description().
  const char* description() const final {
    return "Defines a set of agents as the target by intersecting together one or more child agent "
           "targets.";
  }

 protected:
  // Inherits docs from AgentEventTargetFactory::instance().
  EventTarget* instance() const final { return new AgentSetIntersectionTarget(); }

  // Inherits docs from AgentEventTargetFactory::setFromXML().
  bool setFromXML(EventTarget* target, TiXmlElement* node,
                  const std::string& behaveFldr) const final;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    The target for an agent-target event which provides the difference of two or more
           AgentEventTarget impliementations.

 %AgentSetDifferenceTarget defines the set difference of sets of agents. The allows the target
 of an event to be defined multiple otherwise independent criteria.

 <h3>XML specification</h3>

 To specify an agent set difference, use the following syntax:

 @code{xml}
 <EventSystem conservative="1">
   <Target type="agent_difference" name="some_name">
     <Target name="subset1" type="[agent target type]" ... />
     <Target name="subset2" type="[agent target type]" ... />
     ...
     <Target name="subsetN" type="[agent target type]" ... />
   </Target>
   <Event name="some_event">
     <Triger ... />
     <Response effect="makeFatter" target="some_name" />
   </Event>
 </EventSystem>
 @endcode

 The `agent_difference` type has no unique parameters. However, it does have one or more child tags.
 Each child tag must, in turn, be an implementaiton of an AgentEventTarget. For sets A, B, C, and D
 the resulting set will be `(((A - B) - C) - D)` or, equivalently, `A - (B U C U D)`. Omitting child
 targets is not an error, but will emit a warning.
 */
class MENGE_API AgentSetDifferenceTarget final : public AgentEventTarget {
 public:
  ~AgentSetDifferenceTarget();

  // Inherits docs from EventTarget::finalize().
  void finalize() final;

 protected:
  friend class AgentSetDifferenceTargetTargetFactory;

  // Inherits docs from EventTarget::finalize().
  void doUpdate() final;

  /*!
   @brief    The set of agent targets to process.
   */
  std::vector<AgentEventTarget*> _agent_targets;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    Factory for the AgentSetDifferenceTarget.
 */
class MENGE_API AgentSetDifferenceTargetTargetFactory final : public AgentEventTargetFactory {
 public:
  // Inherits docs from AgentEventTargetFactory::name().
  const char* name() const final { return "agent_difference"; }

  // Inherits docs from AgentEventTargetFactory::description().
  const char* description() const final {
    return "Defines a set of agents as the target by subtracting all subsequent sets from the "
           "first child agent targets.";
  }

 protected:
  // Inherits docs from AgentEventTargetFactory::instance().
  EventTarget* instance() const final { return new AgentSetDifferenceTarget(); }

  // Inherits docs from AgentEventTargetFactory::setFromXML().
  bool setFromXML(EventTarget* target, TiXmlElement* node,
                  const std::string& behaveFldr) const final;
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    The target for an agent-target event which provides the negation of an AgentEventTarget
           impliementations.

 %AgentSetNegationTarget defines a set of agents as W - S, where W is the set of all agents in the
 world and S is the single child set on which this target is defined.

 <h3>XML specification</h3>

 The negation operator takes a single operand (an agent set target) that must be defined inline as
 a child tag.

 @code{xml}
 <EventSystem conservative="1">
   <Target type="agent_negation" name="negated_set">
     <Target name="subset1" type="[agent target type]" ... />
   </Target>
   <Event name="some_event">
     <Triger ... />
     <Response effect="makeFatter" target="negated_set" />
   </Event>
 </EventSystem>
 @endcode
 
 Failure to provide the child tag is considered an error.
 */
class MENGE_API AgentSetNegationTarget final : public AgentEventTarget {
 public:
  ~AgentSetNegationTarget();

  // Inherits docs from EventTarget::finalize().
  void finalize() final;

 protected:
  friend class AgentSetNegationTargetFactory;

  // Inherits docs from EventTarget::finalize().
  void doUpdate() final;

  /*!
   @brief    The set of agent targets to process -- this target *owns* this target.
   */
  AgentEventTarget* _agent_target{};
};

/////////////////////////////////////////////////////////////////////

/*!
 @brief    Factory for the AgentSetNegationTarget.
 */
class MENGE_API AgentSetNegationTargetFactory final : public AgentEventTargetFactory {
 public:
  // Inherits docs from AgentEventTargetFactory::name().
  const char* name() const final { return "agent_negation"; }

  // Inherits docs from AgentEventTargetFactory::description().
  const char* description() const final {
    return "Defines a set of agents as the target as all of the agents in the world *not* in the "
           "set defined by the child agent target.";
  }

 protected:
  // Inherits docs from AgentEventTargetFactory::instance().
  EventTarget* instance() const final { return new AgentSetNegationTarget(); }

  // Inherits docs from AgentEventTargetFactory::setFromXML().
  bool setFromXML(EventTarget* target, TiXmlElement* node,
                  const std::string& behaveFldr) const final;
};

}  // namespace Menge
