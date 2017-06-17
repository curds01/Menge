#include "UplAgent.h"
#include "UplSimulator.h"
#include "MengeCore/Math/geomQuery.h"
#include "MengeCore/Math/consts.h"

namespace UPL {
	using Menge::Agents::BaseAgent;
	using Menge::Agents::Obstacle;
  using Menge::Math::sqr;
	using Menge::Math::Vector2;
	using Menge::INFTY;

	const float EPS = 1e-7f;

	////////////////////////////////////////////////////////////////
	//					Implementation of UPL::Agent
	////////////////////////////////////////////////////////////////

	const std::string Agent::NAME = "upl";

	////////////////////////////////////////////////////////////////

	// mass = 1 Kg, arbitrary default value. The source paper seems to imply a unit mass.
	Agent::Agent() : BaseAgent() {
		_mass = 1.f;
	}

	////////////////////////////////////////////////////////////////

	Agent::~Agent() {
	}

	////////////////////////////////////////////////////////////////

	void Agent::computeNewVelocity() {
		Vector2 force( drivingForce() );
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

    Vector2 force( 0.0f, 0.0f );
    Vector2 x = other->_pos - _pos;
    const float dist_sqd = x * x;

    // Too far away to impart a force.
    if ( dist_sqd > sqr( _neighborDist ) ) return force;

    const Vector2 v = _vel - other->_vel;
    // Diverging; no force necessary.
    if ( v * x < 0 ) return force;

    // TODO: This is dissatisfying; the more the agents collide, the less they are penalized.
    float radius_sqd = sqr( _radius + other->_radius );
    if ( radius_sqd > dist_sqd ) {
      radius_sqd = sqr( other->_radius + _radius - x.Length() );
    }

    Vector2 gradient;
    float t = getDiskGradient( x, radius_sqd, v, &gradient );
    if ( t > 0 ) {
      force = forceMagnitude( t, v ) * gradient;
    }
    return force;
  }

	////////////////////////////////////////////////////////////////

	Vector2 Agent::obstacleForce( const Obstacle * obst ) const {
    Vector2 force( 0.0f, 0.0f );
#if 0
    Vector2 p0 = obst->getP0();
    Vector2 pos_relative = _pos - p0;
    Vector2 n = obst->normal();

    // Negative *convergence* speed means the agent is moving away.
    float converge_speed = -_vel * n;

    if ( pos_relative * n < 0 || converge_speed <= 0 ) {
      // The agent center is already inside the obstacle or the agent is moving away; generate no
      // force.
      return force;
    }

    // Global constants
    const float k = Simulator::_k;
    const float t0 = Simulator::_t0;
    const float m = Simulator::_m;

    // Agent's center is "outside"; determine which feature is nearest.
    float dot_product = pos_relative * obst->_unitDir;
    if ( dot_product >= 0 && dot_product < obst->_length ) {
      // Closest to the line segment.
      Vector2 point_on_segment = p0 + dot_product * obst->_unitDir;
      Vector2 w = _pos - point_on_segment;
      const float dist_sq = w * w;
      // The obstacle is too far away.
      if ( dist_sq > _neighborDist * _neighborDist ) return force;
      const float dist = dist_sq < _radius * _radius ? 0 : sqrtf(dist_sq) - _radius;
      const float t = dist / converge_speed;
      if ( t > 0 ) force = k * (float)exp( -t / t0 ) / powf( t, m ) * ( m / t + 1 / t0 ) * n;
    } else {
      // Closest to an end point; calculate relative to the nearest point.
      float a = _vel * _vel;
      if ( a > EPS ) {
        Vector2 w;
        if ( dot_product > obst->_length ) {
          // Closest to the circle at p1. Rely on the *other* obstacle that shares this point to
          // compute the force.
          return force;
          //w = obst->getP1() - _pos;
        } else if ( dot_product < 0 ) {
          // Closest to the circle at p0.
          w = p0 - _pos;
        }
        const float dist_sq = w * w;
        // The obstacle is too far away.
        if ( dist_sq > _neighborDist * _neighborDist ) return force;
        float radius_sq = _radius * _radius;
        radius_sq = dist_sq < radius_sq ? dist_sq : radius_sq;
        float b = w * _vel;
        float c = dist_sq - radius_sq;
        float discr = b * b - a * c;
        if ( discr > .0f ) {
          float discr_root = sqrtf( discr );
          const float t = ( b - discr_root ) / a;
          if ( t > 0 ) {
            // NOTE: discCollision can only be set to *true* if a > EPS. This is valid.
            force = -k * (float)exp( -t / t0 ) *
              ( _vel - ( b * _vel - a * w ) / discr_root ) /
              ( a * powf( t, m ) ) * ( m / t + 1 / t0 );
          }
        }
      }
    }
    return force;
#else
		Vector2 nearPt;	// set by distanceSqToPoint
		float d_w;	// set by distanceSqToPoint
		if ( obst->distanceSqToPoint( _pos, nearPt, d_w ) == Obstacle::LAST ) return force;
		Vector2 n_w = nearPt - _pos;

    // Agent is moving away from obstacle or obstacle too far away
    if ( _vel * n_w < 0 || d_w > _neighborDist * _neighborDist ) {
      return Vector2( 0.f, 0.f );
    }

		// correct the radius, if the agent is already colliding	
		const float radius = d_w < (_radius * _radius) ? sqrtf( d_w ) : _radius; 

		// Zero speed is handled in the test below (a > EPS)
    const float a = _vel * _vel;
		bool discCollision = false, segmentCollision = false;
		float t_min = INFTY;

		float c, b, discr; 
		float b_temp, discr_temp, c_temp, D_temp;
		Vector2 w_temp, w, o1_temp, o2_temp, o_temp, o, w_o;

		// time-to-collision with disc_1 of the capped rectangle (capsule). Don't bother testing
    // disc_2; it will be covered by the adjacent obstacle.
    if ( a > EPS ) {
      w_temp = obst->getP0() - _pos;
      b_temp = w_temp * _vel;
      c_temp = w_temp*w_temp - ( radius * radius );
      discr_temp = b_temp * b_temp - a * c_temp;
      if ( discr_temp > .0f ) {
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

      //// time-to-collision with disc_2 of the capsule
      //w_temp = obst->getP1() - _pos;
      //b_temp = w_temp * _vel;
      //c_temp = w_temp * w_temp - ( radius * radius );
      //discr_temp = b_temp * b_temp - a * c_temp;
      //if ( discr_temp > .0f ) {
      //  discr_temp = sqrtf( discr_temp );
      //  const float t = ( b_temp - discr_temp ) / a;
      //  if ( t > 0 && t < t_min ) {
      //    t_min = t;
      //    b = b_temp;
      //    discr = discr_temp;
      //    w = w_temp;
      //    c = c_temp;
      //    discCollision = true;
      //  }
      //}
    }

		// time-to-collision with segment_1 of the capsule
		o1_temp = obst->getP0() + radius * obst->normal();
		o2_temp = obst->getP1() + radius * obst->normal();
		o_temp = o2_temp - o1_temp;

		D_temp = det(_vel, o_temp);
		if ( fabs(D_temp) > EPS ) {
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

    // This should never happen; if I'm properly "outside" the obstacle, the offset curve in the
    // normal direction should always be closer.
		//// time-to-collision with segment_2 of the capsule
		o1_temp = obst->getP0() - radius * obst->normal();
		o2_temp = obst->getP1() - radius * obst->normal();
		o_temp = o2_temp - o1_temp;

		D_temp = det(_vel, o_temp);
    if ( fabs( D_temp ) > EPS ) {
			float inverseDet = 1.0f / D_temp;	
			float t = det(o_temp, _pos - o1_temp) * inverseDet;
			float s = det(_vel,_pos - o1_temp) * inverseDet;
			if ( t > 0 && s >= 0 && s <= 1 && t < t_min ) {
        std::cout << "Agent " << _id << " used the far segment at " << Menge::SIM_TIME << "\n";
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
      // NOTE: discCollision can only be set to *true* if a > EPS. This is valid.
		  force = -k * (float)exp(-t_min / t0) *
			  (_vel - (b * _vel - a * w) / discr) /
			  (a * powf( t_min,m) ) * (m / t_min + 1 /t0 );		
		} else if (segmentCollision) {
		  force = k * (float)exp(-t_min / t0) /
			  (powf(t_min, m) * det(_vel, o)) *
			  (m / t_min + 1 / t0) * Vector2(-o._y, o._x);
		}

		return force;
#endif
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

  float Agent::getDiskGradient( const Vector2& center, float rad_sqd, const Vector2& v,
                                Vector2* grad ) const {
    grad->set( 0.f, 0.f );
    const float a = v * v;
    if ( a > EPS ) {
      const float b = center * v;
      const float c = center * center - rad_sqd;
      const float discr = b * b - a * c;
      if ( discr > 0.f ) {
        const float d = sqrtf( discr );
        grad->set( v - ( b * v - a * center ) / d );
        return ( b - d ) / a;
      }
    }
    return -1;
  }

	////////////////////////////////////////////////////////////////

  float Agent::forceMagnitude( float tau, const Vector2& v ) const {
    const float k = Simulator::_k;
    const float t0 = Simulator::_t0;

    // NOTE: The paper advocates t^2, so that's what is used here.
    return -k * (float)exp( -tau / t0 ) / ( ( v * v ) * tau * tau ) * ( 2 / tau + 1 / t0 );
  }

}	// namespace UPL
