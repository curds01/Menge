#pragma once

#include "MixedConfig.h"

#include "MengeCore/Agents/Events/AgentEventTarget.h"

namespace Mixed {

	/*!
	 *	@brief		The target used by the MixedTask to determine which agents will be modified
	 *				by the event controlled by the MixedTask.
	 */
	class MIXED_API MixedTarget final : public Menge::AgentEventTarget {
	public:
		/*! @brief	Clears the agents in the target set. */
		void clear() { _elements.clear(); }

		/*! @brief  Adds the given agent to the target set. */
		void addAgent(Menge::Agents::BaseAgent* agent) {
			_elements.push_back(agent);
		}
	};

} // namespace Mixed
