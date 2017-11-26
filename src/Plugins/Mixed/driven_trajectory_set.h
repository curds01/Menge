#pragma once

#include <unordered_map>
#include <vector>

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/Vector2.h"
#include "MengeCore/resources/Resource.h"

namespace Mixed {

	/*!
		@brief  The structure which reports the status of the driven agents. Shared by the task,
		the event apparatus, and the velocity component.

		The task is responsible for definining the memership. The velocity comnponent applies the
		velocity stored in this structure to the corresponding agent. The events use this to
		add agents in and remove agents from the active set.

		The work flow is as follows:
			- The task is evaluated:
				- it assigns velocitie to all active agents (mapping from agent -> velocity).
				- any agents that are to be retired are put in the retired set (there should be
				  no velocity for that agent.)
				- any agents that need to be included are indicated with a position (agent ->
				  position map). Finally, they should *also* have a velocity in the velocity map.
			- Events are evaluated
				- Two triggers:
					- the restore trigger determines if there are any agents in the restore set and
					  triggers if there are.
					  - The event target is *implicitly* those agents.
					  - The event effect is to teleport them and change their state.
					- The retire trigger determines if there are any agents in the retire set and
					  triggers if there are.
					  - The event target is *implicitly* those agents.
					  - The event effect moves them to the hold state (which includes teleport and
					    various actions.
			- During preferred velocity computation
				- The velocity component finds all agents in the active set and replaces their
				  preferred velocity with the recorded velocity here. All other agents remain
				  unaffected.
	*/
	class DrivenTrajectorySet : public Menge::Resource {
	public:
		const std::string& getLabel() const override { return LABEL; }

		/*! @brief	The unique label for this data type to be used in resource management. */
		static const std::string LABEL;

		/*! @name  Working with the active set of agents. */
		//@{

		/*! @brief  Reports true if there are any active agents. */
		bool has_active_agents() const { return _agent_velocities.size() > 0; }

		/*! @brief  Reports true if the given agent is an active agent. */
		bool is_active(const Menge::Agents::BaseAgent* agent) const {
			return _agent_velocities.count(agent->_id) > 0;
		}

		/*! @brief  Reports the specified velocity for the given agent (if defined). Throws an
		    out-of-range exception if the agent is not active. */
		Menge::Math::Vector2 get_active_velocity(const Menge::Agents::BaseAgent* agent) const {
			return _agent_velocities.at(agent->_id);
		}

		/*! @brief  Sets the driven velocity for the given agent. */
		void set_active_velocity(const Menge::Agents::BaseAgent* agent,
								 const Menge::Math::Vector2& vel) {
			_agent_velocities[agent->_id] = vel;
		}

		void restore_agent(const size_t, const Menge::Math::Vector2& vel);

		/*! @brief  Clears all agents and their velocities from the set. */
		void clear_velocities() {
			_agent_velocities.clear();
		}

		//@}

		/*! @name  Working with agents to restore. */
		//@{

		/*! @brief  Reports true if there are any agents to restore. */
		bool has_agents_to_restore() const { return _restore_agents.size() > 0; }

		/*! @brief  Reports the id of the first agent to restore. The value will be 
		 INVALID_AGENT_ID if there is no agent to restore. */
		size_t peek_restore_agent();

		/*! @brief  Adds an agent, indicated by id, to restore with its target position. */
		void push_restore_agent(size_t id, const Menge::Math::Vector2& position);
		
		//@}

		/*! @name  Working with agents to retire. */
		//@{

		/*! @brief  Reports true if there are any agents to retire. */
		bool has_agents_to_retire() const { return _retire_agents.size() > 0; }

		//@}

		// TODO: The following operaitons
		//	Add agent to restoration
		//  Det

	private:
		// The agents to retire.
		std::vector<size_t> _retire_agents;

		// The agents (and their target positions) to restore.
		std::unordered_map<size_t, Menge::Math::Vector2> _restore_agents;

		// The velocities for all active agents.
		std::unordered_map<size_t, Menge::Math::Vector2> _agent_velocities;
	};

}  // namespace Mixed
