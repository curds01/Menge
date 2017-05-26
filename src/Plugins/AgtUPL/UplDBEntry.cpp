#include "UplDBEntry.h"
#include "UplSimulator.h"
#include "UplInitializer.h"
#include "MengeCore/Runtime/SimulatorDB.h"

namespace PowerLaw {

	using Menge::Agents::SimulatorInterface;

	/////////////////////////////////////////////////////////////////////////////
	//                     Implementation of PowerLaw::DBEntry
	/////////////////////////////////////////////////////////////////////////////

	::std::string DBEntry::briefDescription() const {
		return "Simulator based on Universal Power Law pedestrian model";
	}
	/////////////////////////////////////////////////////////////////////////////

	::std::string DBEntry::longDescription() const {
		return "Simulator based on Universal Power Law pedestrian model\n"
		  "\tThe model is based on Karamouzas et al's 2014 paper\n";
	}

	/////////////////////////////////////////////////////////////////////////////

	::std::string DBEntry::viewerName() const {
		return "UPL ";
	}

	/////////////////////////////////////////////////////////////////////////////
	 
	SimulatorInterface * DBEntry::getNewSimulator() {
		return new Simulator();
	}

	/////////////////////////////////////////////////////////////////////////////
	 
	Menge::Agents::AgentInitializer * DBEntry::getAgentInitalizer() const {
		return new AgentInitializer();
	}

	/////////////////////////////////////////////////////////////////////////////
 
}	// namespace PowerLaw 
