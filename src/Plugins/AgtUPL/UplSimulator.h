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

#ifndef __UPL_SIMULATOR_H__
#define __UPL_SIMULATOR_H__

/*!
 *  @file       UplSimulator.h
 *  @brief	    Contains the Upl::Simulator class. 
 *              Implements the Universal Power Law pedestrian model:
 *              Karamouzas et al, 2014        
 */

#include "UplAgent.h"
#include "MengeCore/mengeCommon.h"
#include "MengeCore/Agents/SimulatorBase.h"

/*!
 *	@namespace    UPL
 *	@brief	      The namespace for the Universal Power Law pedestrian model.
 *                    Implements the Universal Power Law pedestrian model:
 *                    Karamouzas et al, 2014        
 */
namespace UPL {
	/*!
	 *  @brief      Defines the simulator operating on a UPL::Agent.
	 */
	class Simulator : public Menge::Agents::SimulatorBase< Agent > {
	public:
		/*!
		 *  @brief      Constructor.
		 */
		Simulator() : Menge::Agents::SimulatorBase< Agent >() {}

		/*!
		 *	@brief			Reports if there are non-common Experiment parameters that
		 *					this simulator requires in the XML file.
		 *	@returns		By default, the simulator base ONLY uses common parameters.
		 *					Always returns false.
		 */
		virtual bool hasExpTarget() { return true; }

		/*!
		 *  @brief			Reports if the given Experiment attribute tag name belongs to this
		 *					simulator.
		 *  @param		tagName		the name of the considered tag
		 *  @returns		By default, the simulator base ONLY uses common parameters.
		 *					Always returns false.
		 */
		virtual bool isExpTarget( const std::string & tagName ) { return tagName == "UPL"; }

		/*!
		 *	@brief			Given an Experiment parameter name and value, sets the appropriate
		 *					simulator parameter.
		 *	@param			paramName		A string containing the parameter name for the experiment.
		 *	@param			value			A string containing the value for the parameter.
		 *	@returns		whether or not parameters were successfully set
		 */
		virtual bool setExpParam( const std::string & paramName, const std::string & value )
      throw( Menge::Agents::XMLParamException );

	protected:
		friend class Agent;

		/// The scaling constant k of the inter-agent force term.
		static float _k;

		/// The exponential cutoff term tau_0
		static float _t0;

		/// The exponent of the power law (m = 2 in our analysis)
		static float _m;

		/// Relaxation time for the driving force
		static float _ksi;

	};
}	// namespace UPL
#endif	 // __UPL_SIMULATOR_H__
