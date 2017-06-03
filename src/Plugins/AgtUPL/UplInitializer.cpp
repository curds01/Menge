#include "UplInitializer.h"
#include "UplAgent.h"
#include "MengeCore/Math/RandGenerator.h"

namespace UPL {

	using Menge::Agents::BaseAgent;
	using Menge::Math::ConstFloatGenerator;
	using Menge::Logger;
	using Menge::logger;

	////////////////////////////////////////////////////////////////
	//			Implementation of UPL::AgentInitializer
	////////////////////////////////////////////////////////////////

  // mass = 1 Kg, arbitrary default value. The source paper seems to imply a unit mass.
  const float MASS = 1;			///< The agent's default mass.
	
	////////////////////////////////////////////////////////////////

	AgentInitializer::AgentInitializer() : Menge::Agents::AgentInitializer() { 
		_mass = new ConstFloatGenerator( MASS );
	}

	////////////////////////////////////////////////////////////////

	AgentInitializer::AgentInitializer( const AgentInitializer & init) : 
		Menge::Agents::AgentInitializer(init) { 
		_mass = init._mass->copy();
	}

	////////////////////////////////////////////////////////////////

	AgentInitializer::~AgentInitializer() {
		delete _mass;
	}

	////////////////////////////////////////////////////////////////

	bool AgentInitializer::setProperties( BaseAgent * agent ) {
		Agent * a = dynamic_cast< Agent * >( agent );
		if ( a == 0x0 ) return false;
		a->_mass = _mass->getValue();
		return Menge::Agents::AgentInitializer::setProperties( agent );
	}

	////////////////////////////////////////////////////////////////

	bool AgentInitializer::isRelevant( const ::std::string & tagName ) {
	  return ( tagName == "UPL" ) || Menge::Agents::AgentInitializer::isRelevant( tagName );
	}

	////////////////////////////////////////////////////////////////

  Menge::Agents::AgentInitializer::ParseResult AgentInitializer::setFromXMLAttribute(
    const ::std::string & paramName, const ::std::string & value ) {
		ParseResult result = IGNORED;
		if ( paramName == "mass" ) {
			result = constFloatGenerator( _mass, value );
		}

		if ( result == FAILURE ) {
			logger << Logger::WARN_MSG << "Attribute \"" << paramName <<
        "\" had an incorrectly formed value: \"" << value << "\".  Using default value.";
			result = ACCEPTED;
		} else if ( result == IGNORED ){
			return Menge::Agents::AgentInitializer::setFromXMLAttribute( paramName, value );
		} 
		return result;
	}

	////////////////////////////////////////////////////////////////

	AgentInitializer::ParseResult AgentInitializer::processProperty( ::std::string propName,
                                                                   TiXmlElement * node ) {
		ParseResult result = IGNORED;
		if ( propName == "mass" ) {
      result = getFloatGenerator( _mass, node );
		}

		if ( result == FAILURE ) {
			logger << Logger::ERR_MSG << "Error extracting value distribution from Property " <<
        propName << ".";
			return result;
		} else if ( result == IGNORED ) {
			return Menge::Agents::AgentInitializer::processProperty( propName, node );
		}
		return result;
	}

	////////////////////////////////////////////////////////////////

	void AgentInitializer::setDefaults() {
		if ( _mass ) delete _mass;
		_mass = new ConstFloatGenerator( MASS );
		Menge::Agents::AgentInitializer::setDefaults();
	}

	////////////////////////////////////////////////////////////////

}	// namespace UPL
