/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill. 
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation 
for educational, research, and non-profit purposes, without fee, and without a 
written agreement is hereby granted, provided that the above copyright notice, 
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North 
Carolina at Chapel Hill. The software program and documentation are supplied "as is," 
without any accompanying services from the University of North Carolina at Chapel 
Hill or the authors. The University of North Carolina at Chapel Hill and the 
authors do not warrant that the operation of the program will be uninterrupted 
or error-free. The end-user understands that the program was developed for research 
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE 
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY 
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY 
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND 
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "JohanssonAgent.h"
#include "JohanssonSimulator.h"

namespace Johansson {

	using Menge::Agents::BaseAgent;
	using Menge::Agents::Obstacle;
	using Menge::Math::Vector2;

	////////////////////////////////////////////////////////////////
	//					Implementation of Johansson::Agent
	////////////////////////////////////////////////////////////////

	const std::string Agent::NAME = "johannson";

	////////////////////////////////////////////////////////////////
	// direction weight value comes from Johansson's 2007 paper
	Agent::Agent(): BaseAgent(), _dirWeight(0.16f) {
	}

	////////////////////////////////////////////////////////////////

	Agent::~Agent() {
	}

	////////////////////////////////////////////////////////////////

	void Agent::computeNewVelocity() {
		const float TAU = Simulator::REACTION_TIME;
		const float STEP_TIME = Simulator::STRIDE_TIME;
		float B = Simulator::FORCE_DISTANCE;

		// The equation from the paper is as follows (with alpha and beta substituted for i and j):
		//
		// fᵢⱼ = A⋅wᵢⱼ⋅exp(-bᵢⱼ/B)⋅(‖dᵢⱼ‖+‖dᵢⱼ - yᵢⱼ‖)/(2bᵢ)⋅½(dᵢⱼ/‖dᵢⱼ‖ + (dᵢⱼ - yᵢⱼ)/‖dᵢⱼ - yᵢⱼ‖)
		//
		//   Where:
		//		dᵢⱼ = rᵢ - rⱼ
		//		yᵢⱼ = vⱼ⋅Δt⋅eᵢⱼ, 
		//		eᵢ = "direction of motion of pedestrian i"
		//		wᵢⱼ = λᵢ + (1 - λᵢ)⋅½(1 + cos(eᵢ⋅d̂ᵢⱼ)), a directional scale factor
		//		2⋅bᵢⱼ = √((‖dᵢⱼ‖+‖dᵢⱼ - yᵢⱼ‖)²-(vⱼ⋅Δt)²)
		//
		//	Maps to the following variables:
		//		A = Simulator::AGENT_SCALE
		//		wᵢⱼ = w_ij
		//		λᵢ = _dirWeight
		//		B = Simulator::FORCE_DISTANCE
		//		eᵢ = 

		// driving force
		Vector2 force( ( _velPref.getPreferredVel() - _vel ) / TAU );
		// agent forces
		float A = Simulator::AGENT_SCALE;
		for ( size_t i = 0; i < _nearAgents.size(); ++i ) {
			const BaseAgent * otherBase = _nearAgents[i].agent;
			const Agent * const other = static_cast< const Agent *>( otherBase );

			Vector2 d_ij = _pos - other->_pos;
			float dist = abs( d_ij );
			Vector2 relDir = d_ij / dist;
			// directional weight of force
			
			float cosTheta = relDir * _orient;
			float w_ij = _dirWeight + (1.f - _dirWeight) * (1 + cosTheta) * 0.5f;
			float magnitude = A * w_ij;

			// elliptical term
			Vector2 stepOffset = other->_vel * STEP_TIME;
			Vector2 relPosOffset = d_ij - stepOffset;
			float relPosOffsetDist = abs( relPosOffset );
			float term1 = dist + relPosOffsetDist;
			float offsetDistSq = absSq( stepOffset );
			float twoB = sqrtf( term1 * term1 - offsetDistSq );
			float b = twoB / 2.f;
			// Extra magnitude scaling term
			magnitude *= term1 / twoB;
			magnitude *= expf( -b / B );
			// Force direction
			Vector2 forceDir = 0.5f * ( relDir + ( relPosOffset / relPosOffsetDist ) );
			force += magnitude * forceDir;
		}

		// wall forces
		A = Simulator::OBST_SCALE;
		for ( size_t i = 0; i < _nearObstacles.size(); ++i ) {
			Vector2 nearPt;	// set by distanceSqToPoint
			float distSq;	// set by distanceSqToPoint
			if ( _nearObstacles[ i ].obstacle->distanceSqToPoint( _pos, nearPt, distSq ) == 
				 Obstacle::LAST ) continue;
			float dist = sqrtf( distSq );

			Vector2 d_ij = _pos - nearPt;
			Vector2 relDir = d_ij / dist;
			// directional weight of force
			
			float cosTheta = relDir * _orient;
			// NOTE: Below I have 1 - cosTheta instead of 1 + cosTheta
			//	The reason is that I have relDir defined in the opposite direction
			float magnitude = A * ( _dirWeight + (1.f - _dirWeight) * ( 1 - cosTheta ) * 0.5f );

			// Assuming stationary wall - elliptical term goes to distance
			magnitude *= expf( -dist / B );
			// Force direction is just relative direction (for stationary wall)
			force += magnitude * relDir;
		}
		// assume unit mass!
		_velNew = _vel + force * Simulator::TIME_STEP;
	}

}	// namespace Johansson
