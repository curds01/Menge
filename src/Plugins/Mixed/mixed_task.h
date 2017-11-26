/*!
 *	@file		mixed_class.h
 *	@brief		Definition of the task responsible for doing the mixed-behavior work.
 */

#pragma once

#include "MixedConfig.h"
#include "mixed_target.h"
#include "mixed_effect.h"

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"

namespace Mixed {

	/*!
	 *	The MixedTask 
	 */
	class MIXED_API MixedTask : public Menge::BFSM::Task {
	public:
		void doWork(const Menge::BFSM::FSM* fsm) throw(Menge::BFSM::TaskException) override;

		std::string toString() const override { return "mixed"; }

		bool isEquivalent(const Menge::BFSM::Task* task) const override;

	protected:
		/*! @brief The event target for those agents that need to be removed from the main area. */
		MixedTarget* _remove_target;

		/*! @brief The event target for those agents that need to be brought back. */
		MixedTarget* _restore_target;

		/*! @brief The event effect for those agents being restored; their target positions should
		 be defined in this effect class. */
		MixedEffect* _restore_effect;
	};
}  // namespace Mixed
