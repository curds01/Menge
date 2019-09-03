#include "MengeCore/Agents/Events/TargetAgentById.h"

#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Core.h"
#include "MengeCore/Runtime/Logger.h"

namespace Menge {

/////////////////////////////////////////////////////////////////////
//          Implementation of TargetAgentById
/////////////////////////////////////////////////////////////////////

void TargetAgentById::doUpdate() {
  Agents::BaseAgent* agent = SIMULATOR->getAgent(_agentId);
  if (agent) {
    _elements.push_back(agent);
  } else {
    // TODO: Add name as a field on the EventTarget class.
    logger << Logger::WARN_MSG << "TargetAgentById event target attempted";
    logger << " to target agent with id " << _agentId << ". No agent existed with that ";
    logger << "identifier.";
  }
}

/////////////////////////////////////////////////////////////////////
//          Implementation of TargetAgentByIdFactory
/////////////////////////////////////////////////////////////////////

TargetAgentByIdFactory::TargetAgentByIdFactory() : AgentEventTargetFactory() {
  _idID = _attrSet.addSizeTAttribute("id", true /*required*/);
}

/////////////////////////////////////////////////////////////////////

bool TargetAgentByIdFactory::setFromXML(EventTarget* target, TiXmlElement* node,
                                        const std::string& specFldr) const {
  TargetAgentById* idTarget = dynamic_cast<TargetAgentById*>(target);
  assert(idTarget != 0x0 &&
         "Trying to set attributes of a target-by-id event target "
         "on an incompatible object");

  if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

  idTarget->_agentId = _attrSet.getSizeT(_idID);

  return true;
}

}  // namespace Menge
