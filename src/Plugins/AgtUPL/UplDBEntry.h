/*!
 *	@file		PowerLawDBEntry.h
 *	@brief		The simulator database entry for the PowerLaw 2000 pedestrian model.
 */

#ifndef __POWERLAW_DB_ENTRY_H__
#define	__POWERLAW_DB_ENTRY_H__

#include "Upl.h"
#include "MengeCore/Runtime/SimulatorDBEntry.h"

namespace PowerLaw {
	/*!
	 *	@brief		The simulator database entry for the PowerLaw 2000 simulator.
	 */
	class DBEntry : public Menge::SimulatorDBEntry {
	public:
		/*!
		 *	@brief	Gives a brief description of the simulator.
		 *
		 *	@returns	A brief description of the simulator and pedestrian
		 *				model.
		 */
		virtual ::std::string briefDescription() const;

		/*!
		 *	@brief	Gives a long description of the simulator.
		 *
		 *	@returns	A long description of the simulator and pedestrian
		 *				model.
		 */
		virtual ::std::string longDescription() const;

		/*!
		 *	@brief	Gives a label to apply to the interactive viewer.
		 *
		 *	@returns	The name for display on the interactive viewer.
		 */
		virtual ::std::string viewerName() const;

		/*!
		 *	@brief		Gives a unique name to be used as a command-line parameter.
		 *
		 *	This name MUST satisfy two constraints:
		 *		- It must contain no spaces.
		 *		- It must be unique from that used by all other simulators.
		 *
		 *	@returns	A single string (with no spaces) that can be used as
		 *				a command line parameter to uniquely identify this model.
		 */
		virtual ::std::string commandLineName() const { return Agent::NAME; }

		/*!
		 *	@brief		Returns a pointer to this model's Simulator instance.
		 *
		 *	This must be overridden by a derived class
		 *
		 *	@returns	A newly instantiated simulator instance of a type appropriate
		 *				for this database entry.
		 */
		virtual Menge::Agents::SimulatorInterface * getNewSimulator();

		/*!
		 *	@brief		Provides an AgentInitializer appropriate to this simulator class.
		 *
		 *	@returns	A pointer to an agent initializer.  The caller is responsible for
		 *				freeing up the memory.
		 */
		virtual Menge::Agents::AgentInitializer * getAgentInitalizer() const;
	};
}	// namespace PowerLaw

#endif	// __POWERLAW_DB_ENTRY_H__
