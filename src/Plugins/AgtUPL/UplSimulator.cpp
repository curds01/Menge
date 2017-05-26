#include "UplSimulator.h"
#include "MengeCore/Runtime/Utils.h"

namespace PowerLaw {

	using Menge::toFloat;
	using Menge::UtilException;
	using Menge::Agents::SimulatorBase;
	using Menge::Agents::XMLParamException;

	////////////////////////////////////////////////////////////////
	//					Implementation of PowerLaw::Simulator
	////////////////////////////////////////////////////////////////

	// These values come directly from the Helbing 2000 paper
	float	Simulator::_k = 1.5f;
	float	Simulator::_t0 = 3.f;
	float	Simulator::_m = 2.0f;
	float	Simulator::_ksi = 0.54f;

	////////////////////////////////////////////////////////////////

	bool Simulator::setExpParam( const std::string & paramName, const std::string & value ) throw( XMLParamException ) {
		try {
			if ( paramName == "agent_scale" ) {
				_k = toFloat( value );
			} else if ( paramName == "expCutoff" ) {
				 _t0 = toFloat( value );
			} else if ( paramName == "exponent" ) {
				 _m = toFloat( value );
			} else if ( paramName == "relaxTime" ) {
				_ksi = toFloat( value );
			} else if ( ! SimulatorBase<Agent>::setExpParam( paramName, value ) ) {
				// Simulator base didn't recognize the parameter either
				return false;
			}
		} catch ( UtilException ) {
			throw XMLParamException( std::string( "PowerLaw parameter \"") + paramName + std::string("\" value couldn't be converted to the correct type.  Found the value: " ) + value );
		}
		return true;
	}
}	//namespace PowerLaw
