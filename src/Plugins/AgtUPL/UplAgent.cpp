#include "UplAgent.h"
#include "UplSimulator.h"
#include "MengeCore/Math/geomQuery.h"
#include "MengeCore/Math/consts.h"

namespace PowerLaw {
	using Menge::Agents::BaseAgent;
	using Menge::Agents::Obstacle;
	using Menge::Math::Vector2;
	using Menge::INFTY;

	const float EPS = 1e-7f;

	////////////////////////////////////////////////////////////////
	//					Implementation of PowerLaw::Agent
	////////////////////////////////////////////////////////////////

	const std::string Agent::NAME = "upl";

	////////////////////////////////////////////////////////////////

	// mass = 80 Kg comes from PowerLaw's 2000 paper
	Agent::Agent() : BaseAgent() {
		_mass = 80.f;
	}

	////////////////////////////////////////////////////////////////

	Agent::~Agent() {
	}

	////////////////////////////////////////////////////////////////

	void Agent::computeNewVelocity() {
		Vector2 force( drivingForce());
		std::cout << "Agent " << _id << " has " << _nearAgents.size() << " neighbors\n";
		for ( size_t i = 0; i < _nearAgents.size(); ++i ) {
			const BaseAgent * otherBase = _nearAgents[i].agent;
			const Agent * const other = static_cast< const Agent *>( otherBase );

			force += agentForce( other );
		}

		for ( size_t obs = 0; obs < _nearObstacles.size(); ++obs ) {
			const Obstacle * obst = _nearObstacles[ obs ].obstacle;
			force += obstacleForce( obst );
		}
		Vector2 acc = force / _mass;
		clamp(acc, _maxAccel);
		_velNew = _vel + acc * Simulator::TIME_STEP;
	}

	////////////////////////////////////////////////////////////////

	Vector2 Agent::agentForce( const Agent * other ) const {
		// TODO: address right of way

		//What should be global variables

		//Local Variables
		Vector2 force(0.0f, 0.0f);
		Vector2 disp =  other->_pos - _pos;
		const float dispSq = disp * disp; // disp.Length() * disp.Length();
		float radiusSq = _radius + other->_radius;
		radiusSq *= radiusSq;

		// if agents are actually colliding use their separation distance 
		if (dispSq < radiusSq) {
			radiusSq = other->_radius + _radius - disp.Length();
			radiusSq *= radiusSq;
		}
 
		const float k = Simulator::_k;
		const float t0 = Simulator::_t0;
		const float m = Simulator::_m;

		const Vector2 v = _vel - other->_vel;
		const float a = v * v;
		const float b = disp * v;	
		const float c = dispSq - radiusSq;
		float discr = b * b - a * c;
		if ( discr > .0f && abs( a ) > EPS ) {
			discr = sqrtf( discr );
			const float t = ( b - discr ) / a;
			if ( t > 0) {
			  force = -k * (float)exp(-t / t0) * (v - (b * v - a * disp) / discr) / (a * powf(t, m)) * (m / t + 1 / t0);	      
			}
		}
		return force;
	}

	////////////////////////////////////////////////////////////////

	Vector2 Agent::obstacleForce( const Obstacle * obst ) const {
		Vector2 force(0.0f, 0.0f);

		Vector2 nearPt;	// set by distanceSqToPoint
		float d_w;	// set by distanceSqToPoint
		if ( obst->distanceSqToPoint( _pos, nearPt, d_w ) == Obstacle::LAST ) return Vector2(0.f,0.f);
		Vector2 n_w = nearPt - _pos;

		// TODO: The test d_w == radiusSqd is absurd; the odds that the agent is ever
		// *perfectly* touching the obstacle is practically non-existant. It also doesn't
		// match the comment (e.g., "already colliding". Already colliding would be:
		// d_w < _radius ** 2.

		// Agent is moving away from obstacle, already colliding or obstacle too far away
		if (_vel * n_w < 0 || d_w == (_radius * _radius) || d_w > (_neighborDist * _neighborDist)) return Vector2(0.f,0.f);

		// correct the radius, if the agent is already colliding	
		const float radius = d_w < (_radius * _radius) ? sqrtf( d_w ) : _radius; 

		// This is ill-formed for stationary agents e.g., a == "0".
		const float a = _vel *_vel;
		bool discCollision = false, segmentCollision = false;
		float t_min = INFTY;

		float c, b, discr; 
		float b_temp, discr_temp, c_temp, D_temp;
		Vector2 w_temp, w, o1_temp, o2_temp, o_temp, o, w_o;

		// time-to-collision with disc_1 of the capped rectangle (capsule)
		w_temp = obst->getP0() - _pos;
		b_temp = w_temp * _vel;	
		c_temp = w_temp*w_temp - (radius * radius);
		discr_temp = b_temp * b_temp - a * c_temp;
		if ( discr_temp > .0f && a > EPS ) {
			discr_temp = sqrtf( discr_temp );
			const float t = ( b_temp - discr_temp ) / a;
			if ( t > 0 ) {
				t_min = t;
				b = b_temp;
				discr = discr_temp;
				w = w_temp;
				c = c_temp;
				discCollision = true;
			}
		}

		// time-to-collision with disc_2 of the capsule
		w_temp = obst->getP1() - _pos;
		b_temp = w_temp * _vel;	
		c_temp = w_temp * w_temp - (radius * radius);
		discr_temp = b_temp * b_temp - a * c_temp;
		if (discr_temp > .0f && a > EPS) {
			discr_temp = sqrtf( discr_temp );
			const float t = (b_temp - discr_temp) / a;
			if ( t > 0 && t < t_min ) {
				t_min = t;
				b = b_temp;
				discr = discr_temp;
				w = w_temp;
				c = c_temp;
				discCollision = true;
			}
		}

		// time-to-collision with segment_1 of the capsule
		o1_temp = obst->getP0() + radius * obst->normal();
		o2_temp = obst->getP1() + radius * obst->normal();
		o_temp = o2_temp - o1_temp;

		D_temp = det(_vel, o_temp);
		if ( D_temp > EPS ) {
			float inverseDet = 1.0f / D_temp;	
			float t = det(o_temp, _pos - o1_temp) * inverseDet;
			float s = det(_vel, _pos - o1_temp) * inverseDet;
			if (t > 0 && s >= 0 && s <= 1 && t < t_min) {
				t_min = t;
				o = o_temp;
				w_o = _pos - o1_temp;
				discCollision = false;
				segmentCollision = true;
			}
		}

		// time-to-collision with segment_2 of the capsule
		o1_temp = obst->getP0() - radius * obst->normal();
		o2_temp = obst->getP1() - radius * obst->normal();
		o_temp = o2_temp - o1_temp;

		D_temp = det(_vel, o_temp);
		if (D_temp > EPS) {
			float inverseDet = 1.0f / D_temp;	
			float t = det(o_temp, _pos - o1_temp) * inverseDet;
			float s = det(_vel,_pos - o1_temp) * inverseDet;
			if ( t > 0 && s >= 0 && s <= 1 && t < t_min ) {
				t_min = t;
				o = o_temp;
				w_o = _pos - o1_temp;
				discCollision = false;
				segmentCollision = true;
			}
		}

		const float k = Simulator::_k;
		const float t0 = Simulator::_t0;
		const float m = Simulator::_m;
		if (discCollision) {
		  force = -k * (float)exp(-t_min / t0) *
			  (_vel - (b * _vel - a * w) / discr) /
			  (a * powf( t_min,m) ) * (m / t_min + 1 /t0 );		
		} else if (segmentCollision) {
		  force = k * (float)exp(-t_min / t0) /
			  (powf(t_min, m) * det(_vel, o)) *
			  (m / t_min + 1 / t0) * Vector2(-o._y, o._x);
		}

		return force;
	}

	////////////////////////////////////////////////////////////////

	Vector2 Agent::drivingForce() const { 
		return ( _velPref.getPreferredVel() - _vel ) / Simulator::_ksi;
	}

	////////////////////////////////////////////////////////////////

	void Agent::clamp (Vector2 &v, float maxValue) {
		float lengthV = v.Length(); 
		if(lengthV > maxValue)
		{
			float mult = (maxValue / lengthV);
			v._x *=  mult;
			v._y *=  mult;
		}
	}

	////////////////////////////////////////////////////////////////

}	// namespace PowerLaw
