/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
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

#include "MengeCore/Agents/Events/AgentEventTargetSetOp.h"

#include <algorithm>
#include <cassert>
#include <iterator>
// TODO(curds01): Figure out why windows didn't accept unordered_set here.
#include <set>
#include <utility>

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/Events/EventTargetDB.h"

#include "thirdParty/tinyxml.h"

namespace Menge {

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetUnionTarget
/////////////////////////////////////////////////////////////////////

AgentSetUnionTarget::~AgentSetUnionTarget() {
  for (AgentEventTarget* target : _agent_targets) {
    delete target;
  }
}

void AgentSetUnionTarget::finalize() {
  for (AgentEventTarget* subset : _agent_targets) {
    subset->finalize();
  }
}

/////////////////////////////////////////////////////////////////////

void AgentSetUnionTarget::doUpdate() {
  std::set<Agents::BaseAgent*> unioned;

  for (AgentEventTarget* subset : _agent_targets) {
    subset->update();
    unioned.insert(subset->begin(), subset->end());
  }

  _elements.clear();
  _elements.insert(_elements.begin(), unioned.begin(), unioned.end());
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetUnionTargetFactory
/////////////////////////////////////////////////////////////////////

bool AgentSetUnionTargetFactory::setFromXML(EventTarget* target, TiXmlElement* node,
                                            const std::string& specFldr) const {
  auto sTarget = dynamic_cast<AgentSetUnionTarget*>(target);
  assert(sTarget != 0x0 &&
         "Trying to set attributes of a agent set union event target "
         "on an incompatible object");

  if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

  for (TiXmlElement* child = node->FirstChildElement(); child;
       child = child->NextSiblingElement()) {
    if (child->ValueStr() == "Target") {
      EventTarget* target = EventTargetDB::getInstance(child, specFldr);
      AgentEventTarget* agent_target = dynamic_cast<AgentEventTarget*>(target);
      if (agent_target == nullptr) {
        logger << Logger::ERR_MSG << "The <Target> child tag (line " << child->Row()
               << ") of the agent union target (line " << node->Row()
               << ") is not an agent target.";
        return false;
      }
      sTarget->_agent_targets.push_back(agent_target);
    } else {
      logger << Logger::ERR_MSG << "Unrecognized child tag (line " << child->Row()
             << ") of a agent set union target (line " << node->Row()
             << "). Expected <Target> found <" << child->ValueStr() << ">.";
      return false;
    }
  }

  if (sTarget->_agent_targets.size() < 1) {
    logger << Logger::WARN_MSG << "The agent set union target (line " << node->Row()
           << ") has no child agent target tags.";
  }

  return true;
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetIntersectionTarget
/////////////////////////////////////////////////////////////////////

AgentSetIntersectionTarget::~AgentSetIntersectionTarget() {
  for (AgentEventTarget* target : _agent_targets) {
    delete target;
  }
}

void AgentSetIntersectionTarget::finalize() {
  for (AgentEventTarget* subset : _agent_targets) {
    subset->finalize();
  }
}

/////////////////////////////////////////////////////////////////////

void AgentSetIntersectionTarget::doUpdate() {
  _elements.clear();
  if (!_agent_targets.empty()) {
    std::set<Agents::BaseAgent*> result;
    _agent_targets[0]->update();
    result.insert(_agent_targets[0]->begin(), _agent_targets[0]->end());

    std::set<Agents::BaseAgent*> temp;
    for (int i = 1; i < static_cast<int>(_agent_targets.size()); ++i) {
      AgentEventTarget& target = *_agent_targets[i];
      target.update();
      for (auto itr = target.begin(); itr < target.end(); ++itr) {
        if (result.count(*itr) > 0) {
          temp.insert(*itr);
        }
      }
      result = std::move(temp);
      temp.clear();
    }

    _elements.insert(_elements.begin(), result.begin(), result.end());
  }
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetIntersectionTargetFactory
/////////////////////////////////////////////////////////////////////

bool AgentSetIntersectionTargetFactory::setFromXML(EventTarget* target, TiXmlElement* node,
                                                   const std::string& specFldr) const {
  auto sTarget = dynamic_cast<AgentSetIntersectionTarget*>(target);
  assert(sTarget != 0x0 &&
         "Trying to set attributes of a agent set intersection event target "
         "on an incompatible object");

  if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

  for (TiXmlElement* child = node->FirstChildElement(); child;
       child = child->NextSiblingElement()) {
    if (child->ValueStr() == "Target") {
      EventTarget* target = EventTargetDB::getInstance(child, specFldr);
      AgentEventTarget* agent_target = dynamic_cast<AgentEventTarget*>(target);
      if (agent_target == nullptr) {
        logger << Logger::ERR_MSG << "The <Target> child tag (line " << child->Row()
               << ") of the agent set intersection target (line " << node->Row()
               << ") is not an agent target.";
        return false;
      }
      sTarget->_agent_targets.push_back(agent_target);
    } else {
      logger << Logger::ERR_MSG << "Unrecognized child tag (line " << child->Row()
             << ") of a agent set intersection target (line " << node->Row()
             << "). Expected <Target> found <" << child->ValueStr() << ">.";
      return false;
    }
  }

  if (sTarget->_agent_targets.size() < 1) {
    logger << Logger::WARN_MSG << "The agent set intersection target (line " << node->Row()
           << ") has no child agent target tags.";
  }

  return true;
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetDifferenceTarget
/////////////////////////////////////////////////////////////////////

AgentSetDifferenceTarget::~AgentSetDifferenceTarget() {
  for (AgentEventTarget* target : _agent_targets) {
    delete target;
  }
}

void AgentSetDifferenceTarget::finalize() {
  for (AgentEventTarget* subset : _agent_targets) {
    subset->finalize();
  }
}

/////////////////////////////////////////////////////////////////////

void AgentSetDifferenceTarget::doUpdate() {
  _elements.clear();
  if (!_agent_targets.empty()) {
    std::set<Agents::BaseAgent*> result;
    _agent_targets[0]->update();
    result.insert(_agent_targets[0]->begin(), _agent_targets[0]->end());

    std::set<Agents::BaseAgent*> temp;
    for (int i = 1; i < static_cast<int>(_agent_targets.size()); ++i) {
      AgentEventTarget& target = *_agent_targets[i];
      target.update();
      // TODO: I'm copying from target.begin()/end() into a set here because std::set_difference
      //  requires *both* ranges to be sorted. Right now, target doesn't guarantee sorted order.
      //  Remove this copy and operate directly on target.begin()/end() when it guarantees the
      //  ordering (simply swapping it to a a set would do it).
      std::set<Agents::BaseAgent*> target_set(target.begin(), target.end());
      std::set_difference(result.begin(), result.end(), target_set.begin(), target_set.end(),
                          std::inserter(temp, temp.begin()));
      result = std::move(temp);
      temp.clear();
      _elements.insert(_elements.begin(), result.begin(), result.end());
    }
  }
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetDifferenceTargetTargetFactory
/////////////////////////////////////////////////////////////////////

bool AgentSetDifferenceTargetTargetFactory::setFromXML(EventTarget* target, TiXmlElement* node,
                                                       const std::string& specFldr) const {
  auto sTarget = dynamic_cast<AgentSetDifferenceTarget*>(target);
  assert(sTarget != 0x0 &&
         "Trying to set attributes of a agent set difference event target "
         "on an incompatible object");

  if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

  for (TiXmlElement* child = node->FirstChildElement(); child;
       child = child->NextSiblingElement()) {
    if (child->ValueStr() == "Target") {
      EventTarget* target = EventTargetDB::getInstance(child, specFldr);
      AgentEventTarget* agent_target = dynamic_cast<AgentEventTarget*>(target);
      if (agent_target == nullptr) {
        logger << Logger::ERR_MSG << "The <Target> child tag (line " << child->Row()
               << ") of the agent set difference target (line " << node->Row()
               << ") is not an agent target.";
        return false;
      }
      sTarget->_agent_targets.push_back(agent_target);
    } else {
      logger << Logger::ERR_MSG << "Unrecognized child tag (line " << child->Row()
             << ") of an agent set difference target (line " << node->Row() << "). Expected "
             << "<Target> found <" << child->ValueStr() << ">.";
      return false;
    }
  }

  if (sTarget->_agent_targets.size() < 1) {
    logger << Logger::WARN_MSG << "The agent set difference target (line " << node->Row()
           << ") has no child agent target tags.";
  }

  return true;
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetDifferenceTarget
/////////////////////////////////////////////////////////////////////

AgentSetNegationTarget::~AgentSetNegationTarget() {
  assert(_agent_target != nullptr);
  delete _agent_target;
}

void AgentSetNegationTarget::finalize() {
  assert(_agent_target != nullptr);
  _agent_target->finalize();
}

/////////////////////////////////////////////////////////////////////

void AgentSetNegationTarget::doUpdate() {
  assert(_agent_target != nullptr);
  _elements.clear();
  _agent_target->update();
  // TODO(curds01): Insert all agents in the world.
  // TODO(curds01): Remove all agents from _agent_target.
  throw std::logic_error("AgentSetNegationTarget::doUpdate() not implemented yet");
}

/////////////////////////////////////////////////////////////////////
//          Implementation of AgentSetNegationTargetFactory
/////////////////////////////////////////////////////////////////////

bool AgentSetNegationTargetFactory::setFromXML(EventTarget* target, TiXmlElement* node,
                                               const std::string& specFldr) const {
  auto sTarget = dynamic_cast<AgentSetNegationTarget*>(target);
  assert(sTarget != 0x0 &&
         "Trying to set attributes of a agent set negation event target "
         "on an incompatible object");

  if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

  for (TiXmlElement* child = node->FirstChildElement(); child;
       child = child->NextSiblingElement()) {
    if (child->ValueStr() == "Target") {
      if (sTarget->_agent_target != nullptr) {
        logger << Logger::ERR_MSG << "The agent set negation target (line " << node->Row()
               << ") has multiple child <Target> tags; the second was found on line "
               << child->Row() << ". Only one child <Target> tag is allowed.";
        return false;
      }
      EventTarget* target = EventTargetDB::getInstance(child, specFldr);
      AgentEventTarget* agent_target = dynamic_cast<AgentEventTarget*>(target);
      if (agent_target == nullptr) {
        logger << Logger::ERR_MSG << "The <Target> child tag (line " << child->Row()
               << ") of the agent set negation target (line " << node->Row()
               << ") is not an agent target.";
        return false;
      }
      sTarget->_agent_target = agent_target;
    } else {
      logger << Logger::ERR_MSG << "Unrecognized child tag (line " << child->Row()
             << ") of an agent set negation target (line " << node->Row() << "). Expected "
             << "<Target> found <" << child->ValueStr() << ">.";
      return false;
    }
  }

  if (sTarget->_agent_target == nullptr) {
    logger << Logger::WARN_MSG << "The agent set negation target (line " << node->Row()
           << ") has no child agent target tag.";
  }

  return true;
}

}  // namespace Menge
