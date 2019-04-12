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
 @file    ExplicitAgentGenerator.h
 @brief    An agent generator which creates a set of agents based on an explicit enumeration of
          agent positions.
 */

#ifndef __EXPLICIT_AGENT_GENERATOR_H__
#define __EXPLICIT_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/AgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/AgentGeneratorFactory.h"
#include "MengeCore/mengeCommon.h"

#include <vector>

namespace Menge {

namespace Agents {
/*! Definition of agent generator which produces agents based on explicit enumeration of agent
    positions in an XML file.

 <h3>XML specification</h3>

 To specify an explicit agent generator, use the following syntax:

 @code{xml}
 <Generator type="explicit" displace_dist={"u"|"n"|"c"} ...>
   <Agent p_x="float" p_y="float"/>
   <Agent p_x="float" p_y="float"/>
   ...
   <Agent p_x="float" p_y="float"/>
 </Generator>
 @endcode

 The `displace_dist` attribute is the distribution for position displacement as defined in
 @ref agent_generator_noise "AgentGenerator's XML specification".

 For each `<Agent>` sub-tag, the values `p_x` and `p_y` represent the intial position of that agent
 in simulation space. */
class MENGE_API ExplicitGenerator : public AgentGenerator {
 public:
  ExplicitGenerator();

  // Inherits docs from AgentGenerator::agentCount().
  virtual size_t agentCount() { return _positions.size(); }

  // Inherits docs from AgentGenerator::setAgentPosition().
  virtual void setAgentPosition(size_t i, BaseAgent* agent);

  /*! Adds a new agent with the given `position` to `this` generator. */
  void addPosition(const Vector2& position);

 protected:
  /*! The agent positions parsed from the file. */
  std::vector<Vector2> _positions;
};

//////////////////////////////////////////////////////////////////////////////

/*! The ElementFactory for the ExplicitGenerator. */
class MENGE_API ExplicitGeneratorFactory : public AgentGeneratorFactory {
 public:
  // Inherits from AgentGeneratorFactory::name().
  virtual const char* name() const { return "explicit"; }

  // Inherits from AgentGeneratorFactory::description().
  virtual const char* description() const {
    return "Agent generation is done via an explicit list of agent positions, given "
           "in the XML specification.";
  };

 protected:
  // Inherits from AgentGeneratorFactory::instance().
  AgentGenerator* instance() const { return new ExplicitGenerator(); }

  // Inherits from AgentGeneratorFactory::setFromXML().
  virtual bool setFromXML(AgentGenerator* gen, TiXmlElement* node,
                          const std::string& xml_folder) const;

  /*! Parses an agent position from an `<Agent>` tag.

   @param   node    The XML node containing the agent definition.
   @returns The 2D point defined in the `<Agent`> tag.
   @throws  AgentGeneratorException is the agent tag doesn't provide the required data. */
  Vector2 parseAgent(TiXmlElement* node) const;
};
}  // namespace Agents
}  // namespace Menge
#endif  // __EXPLICIT_AGENT_GENERATOR_H__
