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
 @file    HexLatticeGenerator.h
 An agent generator which creates a set of agents based on the definition of a hexagonal packed
 lattice fit into a rectangle. (i.e., rows are offset for maximal packing). */

#ifndef __LATTICE_AGENT_GENERATOR_H__
#define __LATTICE_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/AgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/AgentGeneratorFactory.h"
#include "MengeCore/mengeCommon.h"

namespace Menge {

namespace Agents {
/*! Definition of an agent generator class which produces agents based on the positions of the
 intersections of a hexagonal lattice bounded by a rectangle.

 Circles can be maximally packed into a hexagonal lattice. The %HexLatticeGenerator creates agents
 positioned at vertices of such a lattice.

 Conceptually, the %HexLatticeGenerator defines an rectangular region (open on one side). It fills
 the rectangular region with agents in a lattice pattern. Some parameters are concerned with how the
 rectangle gets filled, and the remaining parameters define the location and orientation of the
 rectangular region.

 <h3>Filling the rectangular region</h3>

 It works best to think of the rectangular region as a box aligned with a 2D frame. The `width` of
 the box is a user-defined parameter. The height of the box depends on the number and placement of
 agents. The box gets filled, starting with its "front". The front of the box is the edge parallel
 to the x-axis. The parameters that affect how the box gets filled are: `width`, `row_direction`,
 `density`, and `population` (as shown in the figures below).

 <!--
            │            │  ↑
            │            │  │
            │            │  │ filling direction
  ↑ y       │            │  │
  │         │            │  │
  │         │            │
  └──→      ╘════════════╛  ← box front
     x          width
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_1.png

 The `density` setting determines how tightly packed the agents are. It places them so that the
 agents achieve the average given density. The higher the density value, the closer the agents
 become. Given the radius of the agents, it is possible to specify a density that is actually
 greater than the agents can achieve in a collision-free state; in this case, the agents' initial
 positions _will_ be colliding.
 
 <!--
            │            │     │            │
            │            │     │            │
            │            │     │            │
  ↑ y       │ ○  ○  ○  ○ │     │ ○ ○ ○ ○ ○ ○│
  │         │  ○   ○  ○  │     │○ ○ ○ ○ ○ ○ │
  │         │ ○  ○  ○  ○ │     │ ○ ○ ○ ○ ○ ○│
  └──→      ╘════════════╛     ╘════════════╛
     x        low density       high density
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_2.png

 In the hexagonal lattice, there is an inherent "row" direction, in which the agents form an obvious
 row. In the previous figures, the rows are parallel with the x-axis. The rows can equally well be
 defined in the y-axis. The `row_direction` parameter controls this layout by providing one of two
 valid values: "x" or "y" as shown below.
 
 <!--
            │            │  │            │
            │ ○   ○   ○  │  │            │
            │   ○   ○    │  │            │
  ↑ y       │ ○   ○   ○  │  │ ○  ○  ○  ○ │
  │         │   ○   ○    │  │  ○   ○  ○  │
  │         │ ○   ○   ○  │  │ ○  ○  ○  ○ │
  └──→      ╘════════════╛  ╘════════════╛
     x         y-rows           x-rows
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_3.png

 Finally, the `population` _approximately_ specifies how many agents to place. It is not exact; the
 %HexLatticeGenerator fills a _complete_ lattice. So, the total number of agents created will be
 _at least_ the value provided by `population` but with sufficient additional agents to comlete
 the last row. The figure below highlights the agents which correspond to the exact population
 number differently from those added to complete the row.

 <!--
            │            │    │            │    │            │  
            │            │    │            │    │            │  
            │            │    │            │    │            │  
  ↑ y       │ ○   ●   ●  │    │ ○   ○   ●  │    │ ○   ○   ○  │  
  │         │   ○   ○    │    │   ○   ○    │    │   ○   ○    │  
  │         │ ○   ○   ○  │    │ ○   ○   ○  │    │ ○   ○   ○  │  
  └──→      ╘════════════╛    ╘════════════╛    ╘════════════╛  
     x      Population = 6    Population = 7    Population = 8
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_4.png

 <h3>Positioning the rectangular region</h3>

 The position of the open rectangle is defined with respect to an anchor point and an orientation
 specified by the parameters `anchor_x`, `anchor_y`, `alignment`, and `rotation`.

 The rectangular region has its "front" edge parallel with the x-axis and placed at the minimum
 y location (as shown above). The user defines an anchor point, `a = [xₐ, yₐ]` and places the
 leading edge at one of three locations relative to that anchored point.
 
 <!--            │            │    │            │     │            │
            │            │    │            │     │            │
            │            │    │            │     │            │
  ↑ y       │            │    │            │     │            │
  │         │            │    │            │     │            │
  │         │            │    │            │     │            │
  └──→      ╘═════a══════╛    a════════════╛     ╘════════════a
     x          center             left              right
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_5.png

 Finally, the box can be rotated from its default orientation, around its anchored point, a
 specified number of degrees by setting a non-zero `rotation` value (where positive rotation is
 rotation in a counter-clockwise direction).
 
 <!--                           ╲                    ╲  
                  ╲ 
                   ╲
  ↑ y              ╱
  │       ╲       ╱
  │        ╲     ╱
  └──→      ╲   ╱
     x       ╲ ╱╮ θ-degrees
              a┈┈┈┈┈┈┈┈┈
 -->
  @image html Agents/AgentGenerators/image/hex_lattice_6.png

 <h3>XML specification</h3>

 To specify a hexagonal lattice generator, use the following syntax:

 @code{xml}
 <Generator type="hex_lattice"
            anchor_x="float" anchor_y="float"
            alignment={"center"|"left"|"right"} row_direction={"x"|"y"}
            density="float" width="float"
            population="int" rotation="float"
            displace_dist={"u"|"n"|"c"} ... />
 @endcode

 The various parameters have the following interpretation:
   - `anchor_x` and `anchor_y` are the position of the box's anchor point __a__.
   - `alignment` position's the lattice region's front edge on the anchor point. Must be one of
      three values: `center`, `left`, or `right`.
   - `row_direction` determines the row layout of the lattice. Must be `x` or `y`.
   - `density` is the targeted average density of the lattice.
   - `width` the width of the lattice region's front edge.
   - `population` is the target population value.
   - `rotation` is amount of rotation (in degrees) of the lattice region from it's canonical frame.
   - `displace_dist` is the distribution for position displacement as defined in
      @ref agent_generator_noise "AgentGenerator's XML specification".
 */
class MENGE_API HexLatticeGenerator : public AgentGenerator {
 public:
  /*! Specification of how the lattice region's front edge should be aligned to the anchor point. */
  enum AnchorAlignEnum {
    CENTER,       ///< The front edge is centered on the anchor.
    LEFT_CORNER,  ///< The front edge's "left" end is on the anchor.
    RIGHT_CORNER  ///< The front edge's "right" end is on the anchor.
  };

  /*! Specification of which direction the lattice rows run (in the lattice region's canonical
      frame. */
  enum LatticeRowEnum {
    ROW_X,  ///< The rows run parallel with the local x-axis.
    ROW_Y   ///< The rows run parallel with the local y-axis.
  };

  HexLatticeGenerator();

  // Inherits docs from AgentGenerator::agentCount().
  virtual size_t agentCount() { return _totalPop; }

  // Inherits docs from AgentGenerator::setAgentPosition().
  virtual void setAgentPosition(size_t i, BaseAgent* agt);

  /*! Sets all the properties of `this` agent generator.

   @param   anchor          The anchor position of the lattice.
   @param   align           The alignment of the lattice region's front edge.
   @param   dir             The desired row direction.
   @param   width           The width of the bounding region
   @param   density         The target density (agent/m²).
   @param   tgtPopulation   The target population (agents).
   @param   angle           The rotation angle (in degrees). */
  void set(const Vector2& anchor, AnchorAlignEnum align, LatticeRowEnum dir, float width,
           float density, size_t tgtPopulation, float angle);

  /*! Sets the lattice rotation. */
  void setRotationDeg(float angle);

 protected:
  /*! The anchor point of the lattice region. */
  Vector2 _anchor;

  /*! The direction of the row layout. */
  LatticeRowEnum _rowDir;

  /*! The cosine of the amount the lattice is rotated around its anchor point. Positive rotation is
   a counter-clockwise rotation. */
  float _cosRot;

  /*! The sine of the amount the lattice is rotated around its anchor point. Positive rotation is
   a counter-clockwise rotation. */
  float _sinRot;

  /*! The actual number of agents to create based on run-time parameters. */
  size_t _totalPop;

  /*! The distance between rows for the given target density. */
  float _rowDist;

  /*! The distance between neighbors in a single row for the given target density. */
  float _nbrDist;

  /*! The population of the major row (interpreted differently depending on lattice row direction.
   */
  size_t _rowPop;

  /*! The number of rows to create. */
  size_t _rowCount;
};

//////////////////////////////////////////////////////////////////////////////

/*! The ElementFactory for the HexLatticeGenerator. */
class MENGE_API HexLatticeGeneratorFactory : public AgentGeneratorFactory {
 public:
  HexLatticeGeneratorFactory();

  // Inherits from AgentGeneratorFactory::name().
  virtual const char* name() const { return "hex_lattice"; }

  // Inherits from AgentGeneratorFactory::description().
  virtual const char* description() const {
    return "Agent generation is done via the specification of a bounded hexagonal "
           "lattice.";
  };

 protected:
  // Inherits from AgentGeneratorFactory::instance().
  AgentGenerator* instance() const { return new HexLatticeGenerator(); }

  // Inherits from AgentGeneratorFactory::setFromXML().
  virtual bool setFromXML(AgentGenerator* gen, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  /*! The identifier for the "anchor_x" float parameter. */
  size_t _anchorXID;

  /*! The identifier for the "anchor_y" float parameter. */
  size_t _anchorYID;

  /*! The identifier for the "alignment" string parameter. */
  size_t _alignID;

  /*! The identifier for the "row_direction" string parameter. */
  size_t _rowDirID;

  /*! The identifier for the "density" float parameter. */
  size_t _densityID;

  /*! The identifier for the "width" float parameter. */
  size_t _widthID;

  /*! The identifier for the "count" size_t parameter. */
  size_t _popID;

  /*! The identifier for the "rotation" float parameter. */
  size_t _rotID;
};
}  // namespace Agents
}  // namespace Menge
#endif  // __LATTICE_AGENT_GENERATOR_H__
