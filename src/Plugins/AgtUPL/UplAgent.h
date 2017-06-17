
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

The implementation is based on the implementation of the Universal Power Law
pedestrian model, with permission of the authors.
  http://motion.cs.umn.edu/PowerLaw/
*/

/*!
 *	@file		  UplAgent.h
 *	@brief		The agent specification for the pedestrian model based
 *			      on the Universal Power Law pedestrian model.
 *                      Karamouzas et al, 2014        
 */

#ifndef __UPL_AGENT_H__
#define	__UPL_AGENT_H__

#include "MengeCore/Agents/BaseAgent.h"

namespace UPL {
	/*!
	 *	@brief		Agent definition for the Universal Power Law pedestrian model.
	 */
	class Agent : public Menge::Agents::BaseAgent {
	public:
		/*!
		 *  @brief		A variant of the copy constructor.
		 */
		Agent();

		/*!
		 *  @brief      Destroys this agent instance.
		 */
		~Agent();

		/*!
		 *  @brief      Computes the new velocity of this agent.
		 */
		void computeNewVelocity();

		/*!
		 *	@brief		Used by the plugin system to know what artifacts to associate with
		 *				agents of this type.  Every sub-class of must return a globally
		 *				unique value if it should be associated with unique artifacts.
		 */
		virtual std::string getStringId() const { return NAME; }

		/*! @brief	The name identifier for this agent type. */
		static const std::string NAME;

		/*!
		 *	@brief		Compute the force due to another agent
		 *	@param		other			A pointer to a neighboring agent
		 *	@returns	The force imparted by the other agent on this agent
		 */
		Menge::Math::Vector2 agentForce( const Agent * other ) const;

		/*!
		 *	@brief		Compute the force due to a nearby obstacle
		 *	@param		obst			A pointer to the obstacle
		 *	@returns	The force imparted by the obstacle on this agent
		 */
		Menge::Math::Vector2 obstacleForce( const Menge::Agents::Obstacle * obst ) const;

		/*!
		 *	@brief		Computes the driving force for the agent
		 *	@returns	The vector corresponding to the agent's driving force.
		 */
		Menge::Math::Vector2 drivingForce() const;

		/*!
     *  @brief Caps the magnitude of a vector to a maximum value. 
     *  @param force A force vector
		 *  @param maxValue The maximum magnitude of the force.
		 */
		void clamp(Menge::Math::Vector2 &v, float maxValue);

    /*!
     *  @brief  Reports the gradient (w.r.t. position) of the time to collision between a point
     *          and a disk.
     *  @param c          The center of the disk (relative to the query point).
     *  @param rad_sqd    The squared radius of the disk.
     *  @param v          The velocity of the point (relative to the disk).
     *  @param grad       The value of the gradient is written to this member.
     *  @returns          The time to collision; non-negative numbers imply no collision and no 
     *                    gradient.
     */
    float getDiskGradient( const Menge::Math::Vector2& c, float rad_sqd, 
                           const Menge::Math::Vector2& v, Menge::Math::Vector2* grad ) const;

    /*!
     *  @brief    Given the positive time to collision tau, reports the force magnitude scalar
     *            based on the power law definition and global parameters.
     *  @param  tau   The positive time to collision (this is *not* checked).
     *  @param  v     The relative velocity between the two objects.
     */
    float forceMagnitude( float tau, const Menge::Math::Vector2& v ) const;

		/*!
		 *	@brief		The mass of the agent
		 */
		float		_mass;
	};
}	// namespace UPL

#endif  // __UPL_AGENT_H__
