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
*/

/*!
 @file    AgentGenerator.h
 The definition of the agent generator element. Defines the intial numbers and positions of agents
 in the simulation. */
#ifndef __AGENT_GENERATOR_H__
#define __AGENT_GENERATOR_H__

#include "MengeCore/PluginEngine/Element.h"
#include "MengeCore/mengeCommon.h"

namespace Menge {

namespace Agents {
// forward declaration
class BaseAgent;

/*! Exception class for agent generator computation; interpreted as *non*-fatal.  */
class MENGE_API AgentGeneratorException : public virtual MengeException {
 public:
  AgentGeneratorException() : MengeException() {}

  /*! Constructs exception with the given `message`. */
  AgentGeneratorException(const std::string& message) : MengeException(message) {}
};

/*! The *fatal* agent generator exception.  */
class MENGE_API AgentGeneratorFatalException : public AgentGeneratorException,
                                               public MengeFatalException {
 public:
  AgentGeneratorFatalException()
      : MengeException(), AgentGeneratorException(), MengeFatalException() {}

  /*! Constructs exception with the given `message`. */
  AgentGeneratorFatalException(const std::string& message)
      : MengeException(message), AgentGeneratorException(), MengeFatalException() {}
};

/*! The base class for elements that generate agents and their initial positions for simulation
    initialization.

 This is an abstract class for defining the agent generation abstraction. Essentially,
 the AgentGenerator produces a set of agent positions. Its properties should be sufficient to
 produce a count of agents with defined positions. Derived classes provide their own logic for
 determining the number of agents and their positions.

 The base class provides a further mechanism for applying noise to the initial positions of the
 generated agents. This noise is applied "on top" of the derived class's logic -- therefore it is
 not necessary for derived classes to define noise models.

 The displacement is defined by two distributions: 

   - `θ ∈ U(0, 360)`, the direction of displacement is drawn from a uniform distribution of angles.
   - `d ∈ Dist`, the distance of displacement is drawn from a user-specified distribution of
     distances.

 The distance distribution is defined in the XML as a standard, scalar distribution using the 
 prefix "displace_" as shown below:

 @anchor agent_generator_noise
 <h3>XML Specification</h3>

@code{xml}
 <Generator type="TYPE_NAME" ...
            displace_dist="u" displace_min="0.0" displace_max="0.5"
 />
 @endcode

 In this example, the displacement distance is drawn from the uniform distribution `u(0, 0.5)`.
 (The ellipses take the place of the type-specific parameters for the `TYPE_NAME` generator class.) */
class MENGE_API AgentGenerator : public Element {
 public:
  AgentGenerator();

 protected:
  /*! Virtual destructor. */
  virtual ~AgentGenerator();

 public:
  /*! Reports the number of agents that this generator has positions for. */
  virtual size_t agentCount() = 0;

  /*! Sets the position of the given @p agent from the `i`ᵗʰ agent position from this generator.

   @param    i        The index of the requested position in the sequence. Must be in the range
                      [0, agentCount() - 1].
   @param    agent    A pointer to the agent whose position is to be set.
   @throws   AgentGeneratorException if the index, `i`, is invalid. */
  virtual void setAgentPosition(size_t i, BaseAgent* agent) = 0;

  /*! Sets the distribution for the generator's displacement *distance*.

   The %AgentGenerator takes ownership of the float generator provided and is responsible for its
   destruction.

   @param    generator    The generator for displacement distance. */
  void setNoiseGenerator(Math::FloatGenerator* generator);

  /*! Creates a perturbed position value from the given `position` by adding nose from `this`
      generator's displacement distribution.

   @param   position   The position to perturb.
   @returns `position + Dist(x, y)`, the perturbed point. */
  Math::Vector2 addNoise(const Math::Vector2& position);

 protected:
  // The generator for displacement distance; defaults to the distribution `U(0, 0)`.
  Math::FloatGenerator* _disp;

  // The generator for direction of displacement; set to the distribution `U(0, 2π)`.
  Math::UniformFloatGenerator _dir;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __AGENT_GENERATOR_H__
