#pragma once

#include "MixedConfig.h"

#include "MengeCore/Agents/Events/AgentEventEffect.h"

namespace Mixed {

	/*!
	 *	@brief		The event effect responsible for bringing an agent out of the queue and
	 *				into a driven trajectory. It teleports the target agents to a set of mapped
	 *				positions.
	 */
	class MIXED_API MixedEffect final : public Menge::AgentEventEffect {
	public:
		/*!
			@brief	Constructor
			@param	target_state	Specifies the target state
		 */
		MixedEffect(const std::string& target_state);

		bool isCompatible(Menge::EventTarget* target) override;
	protected:
		void agentEffect(Menge::Agents::BaseAgent* agent) override;
	};

}  // namespace Mixed
