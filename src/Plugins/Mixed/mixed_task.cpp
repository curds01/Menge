#include "mixed_task.h"

namespace Mixed {

	using Menge::BFSM::FSM;
	using Menge::BFSM::Task;
	using Menge::BFSM::TaskException;
	using Menge::BFSM::TaskFactory;

	/////////////////////////////////////////////////////////////////////
	//                   Implementation of MixedTask
	/////////////////////////////////////////////////////////////////////

	void MixedTask::doWork(const FSM* fsm) throw(TaskException) {
		// todo: Implement this
		//	1. Determine the required agents
		//		- if too many agents, retire some
		//		- if not enough, revive some
	}

	/////////////////////////////////////////////////////////////////////

	bool MixedTask::isEquivalent(const Task * task) const {
		const MixedTask * other = dynamic_cast<const MixedTask* >(task);
		if (other == 0x0) { 
			return false; 
		} else {
			return _remove_target == other->_remove_target &&
				_restore_effect == other->_restore_effect &&
				_restore_target == other->_restore_target;
		}
	}

}  // namespace Mixed
