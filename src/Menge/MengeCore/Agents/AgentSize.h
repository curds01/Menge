#ifndef __AGENT_SIZE_H__
#define __AGENT_SIZE_H__

/*!
 *	@file		AgentSize.h
 *	@brief		Contains the base description for agent size.
 */

#include "MengeCore/mengeCommon.h"

namespace Menge {
	namespace Agents {
		/*!
		 *	@brief		Base interface for describing an agent's size.  Serves as a flexible way
		 *				of caching, modifying, and restoring agent size parameters in a 
		 *				mdoel-agnostic manner.
		 */
		class MENGE_API AgentSize {
		public:
			/*!
			 *	@brief		Default destructor to satisfy virtual interface.	
			 */
			virtual ~AgentSize() {}

			/*!
			 *	@brief		Queries the size for its clearance size, the amount of space to reserve
			 *				during path planning.
			 */
			virtual float getClearanceSize() const = 0;

			/*!
			 *	@brief		Create a copy of this size offset by the given amount.
			 *
			 *	@param		offset		The amount to offset the agent size.
			 */
			virtual AgentSize * offsetSize( float offset ) const = 0;

			/*!
			 *	@brief		Create a copy of this size scaled by the given amount.
			 *
			 *	@param		scale		The scale factor for the agent size.
			 */
			virtual AgentSize * scaleSize( float scale ) const = 0;
		};
	}  // namespace Agents
}  // namespace Menge

#endif	// __AGENT_SIZE_H__
