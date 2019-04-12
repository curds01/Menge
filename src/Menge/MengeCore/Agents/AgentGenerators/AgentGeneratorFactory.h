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
 @file    AgentGeneratorFactory.h
 The factory for parsing xml data and instantiating agent generator implementations.
 */

#ifndef __AGENT_GENERATOR_FACTORY_H__
#define __AGENT_GENERATOR_FACTORY_H__

#include "MengeCore/Agents/AgentGenerators/AgentGenerator.h"
#include "MengeCore/CoreConfig.h"
#include "MengeCore/PluginEngine/ElementFactory.h"

namespace Menge {

namespace Agents {
/*! An ElementFactory for AgentGenerator types. */
class MENGE_API AgentGeneratorFactory : public ElementFactory<AgentGenerator> {
 protected:
  /*! Inherits from ElementFactory::setFromXML()  */
  virtual bool setFromXML(AgentGenerator* generator, TiXmlElement* node,
                          const std::string& xml_folder) const;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __AGENT_GENERATOR_FACTORY_H__
