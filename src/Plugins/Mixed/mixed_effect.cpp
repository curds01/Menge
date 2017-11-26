#include "mixed_effect.h"

#include "mixed_target.h"

namespace Mixed {

	using Menge::Agents::BaseAgent;
	using Menge::EventTarget;
	
	bool MixedEffect::isCompatible(EventTarget* target) {
		return dynamic_cast<MixedTarget*>(target) != nullptr;
	}

	void MixedEffect::agentEffect(BaseAgent* agent) {
		// TODO: Exit
	}


}  // namespace Mixed
