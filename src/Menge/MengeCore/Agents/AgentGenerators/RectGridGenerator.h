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
 @file    RectGridGenerator.h
 An agent generator which creates a set of agents based on the definition of a rectangular lattice,
 with an agent at each point. */

#ifndef __RECT_GRID_GENERATOR_H__
#define __RECT_GRID_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/AgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/AgentGeneratorFactory.h"
#include "MengeCore/mengeCommon.h"

#include <vector>

namespace Menge {

namespace Agents {
/*! Definition of an agent generator class which produces agents based on the positions of
 intersections on a regular grid.

 Conceptually, the %RectGridGenerator defines a rectangular region. The region is implicitly defined
 by the position of the _anchor_ agent, counts of agents in the x- and y-directions, and spacing
 between agents. Finally, this rectangular region can be arbitrarily oriented by rotating it around
 the anchor agent.

 <h3>Defining the grid</h3>

 The rectangular region is defined parallel with its local xy-axes. One corner of the grid rectangle
 will always be the anchor position `a = [xₐ, yₐ]`. An agent is always located at the anchor
 position. _Which_ corner the anchor is depends on the offset values (`Δx` and `Δy`). It is the
 distance between adjacent agents in the local x- and y-directions, respectively. Depending on the
 _sign_, the anchor point can be any of the four corners.

 <!--
              ┌┄┄┄┄┄┄┄┄┄┄┄┄┐    a → ○┄┄┄┄┄┄┄┄┄┄┄┄┐      ┌┄┄┄┄┄┄┄┄┄┄┄┄○ ← a     ┌┄┄┄┄┄┄┄┄┄┄┄┄┐
              ┆            ┆        ┆            ┆      ┆            ┆         ┆            ┆
              ┆            ┆        ┆            ┆      ┆            ┆         ┆            ┆
  ↑ y         ┆            ┆        ┆            ┆      ┆            ┆         ┆            ┆
  ┆           ┆            ┆        ┆            ┆      ┆            ┆         ┆            ┆
  ┆           ┆            ┆        ┆            ┆      ┆            ┆         ┆            ┆
  └┄┄→    a → ○┄┄┄┄┄┄┄┄┄┄┄┄┘        └┄┄┄┄┄┄┄┄┄┄┄┄┘      └┄┄┄┄┄┄┄┄┄┄┄┄┘         └┄┄┄┄┄┄┄┄┄┄┄┄○ ← a
     x       Δx > 0, Δy > 0         Δx > 0, Δy < 0      Δx < 0, Δy < 0         Δx < 0, Δy > 0
 -->
 @image html Agents/AgentGenerators/image/rect_grid_1.png

 The _size_ of the rectangle depends on the count of the agents to be used in each direction. It is
 always the case that for values `count_x` and `count_y`, there will be exactly `count_x * count_y`
 total agents generated and positioned. They will span a region that is
 `(count_x - 1)Δx` wide and `(count_y - 1)Δy` tall.

 <!--
              ○┄┄○┄┄○┄┄○┄┄○ ┬
              ┆           ┆ │
              ┆           ┆ │ Δy
  ↑ y         ○  ○  ○  ○  ○ ┼
  ┆           ┆           ┆ │
  ┆           ┆           ┆ │ Δy
  └┄┄→    a → ○┄┄○┄┄○┄┄○┄┄○ ┴
     x        ├──┼──┼──┼──┤
               Δx Δx Δx Δx
 -->
 @image html Agents/AgentGenerators/image/rect_grid_2.png

 <h3>Rotating the grid</h3>

 The grid is defined parallel to a local frame. But that frame can be rotated relative to the
 simulation frame via the `rotation` parameter. It represents the amount of rotation in degrees.
 Positive values lead to counter-clockwise rotation. The rectangular region rotates around the
 anchor position.
 <!--                ○               ╱ ╲              ○   ○
             ╱     ╲
            ○   ○   ○
  ↑ y      ╱       ╱
  │       ○   ○   ○
  │        ╲     ╱
  └──→      ○   ○
     x       ╲ ╱╮ θ-degrees
          a → ○┈┈┈┈┈┈┈┈┈
 -->
  @image html Agents/AgentGenerators/image/rect_grid_3.png

 <h3>XML specification</h3>

 To specify an rectangular grid generator, use the following syntax:

 @code{xml}
 <Generator type="rect_grid"
   anchor_x="float" anchor_y="float"
   offset_x="float" offset_y="float"
   count_x="int" count_y="int"
   rotation="float"
 />
 @endcode

 The various parameters have the following interpretation:
 - `anchor_x` and `anchor_y` defined the anchor position `a = [xₐ, yₐ]`.
 - `offset_x` and `offset_y` are the `Δx` and `Δy` values defining the spacing between neighboring
    agents.
 - `count_x` and `count_y` determine the number of rows and columns of agents in the grid.
 - `rotation` is the amount of rotation (in degrees) around the _anchor position_, the grid gets
    rotated. */
class MENGE_API RectGridGenerator : public AgentGenerator {
 public:
  RectGridGenerator();

  // Inherits docs from AgentGenerator::agentCount().
  virtual size_t agentCount() { return _xCount * _yCount; }

  // Inherits docs from AgentGenerator::setAgentPosition().
  virtual void setAgentPosition(size_t i, BaseAgent* agt);

  /*! Sets the anchor position to `p`. */
  void setAnchor(const Vector2& p) { _anchor.set(p); }

  /*! Sets the offset value to `o`. */
  void setOffset(const Vector2& o) { _offset.set(o); }

  /*! Sets the number of agents in the local x-direction to `count`. */
  void setXCount(size_t count) { _xCount = count; }

  /*! Sets the number of agents in the local y-direction to `count`. */
  void setYCount(size_t count) { _yCount = count; }

  /*! Sets the number of agents in the local x- and y-directions to `xCount` and `yCount`,
   respectively. */
  void setAgentCounts(size_t xCount, size_t yCount) {
    _xCount = xCount;
    _yCount = yCount;
  }

  /*! Sets the grid rotation to `angle` degrees; positive values lead to counter-clockwise rotation.
   */
  void setRotationDeg(float angle);

 protected:
  /*! The anchor point of the grid rectangle. */
  Vector2 _anchor;

  /*! The offset from one agent to the next agent (along the local x- and y-axes, respectively). */
  Vector2 _offset;

  /*! The number of columns of agents along the local x-axis. */
  size_t _xCount;

  /*! The number of rows of agents along the local y-axis. */
  size_t _yCount;

  /*! The cosine of the rotation angle around the anchor point. */
  float _cosRot;

  /*! The sine of the rotation angle around the anchor point.  */
  float _sinRot;
};

//////////////////////////////////////////////////////////////////////////////

/*! The ElementFactory for the RectGridGenerator. */
class MENGE_API RectGridGeneratorFactory : public AgentGeneratorFactory {
 public:
  RectGridGeneratorFactory();

  // Inherits from AgentGeneratorFactory::name().
  virtual const char* name() const { return "rect_grid"; }

  // Inherits from AgentGeneratorFactory::description().
  virtual const char* description() const {
    return "Agent generation is done via the specification of a rectangular grid.";
  };

 protected:
  // Inherits from AgentGeneratorFactory::instance().
  AgentGenerator* instance() const { return new RectGridGenerator(); }

  // Inherits from AgentGeneratorFactory::setFromXML().
  virtual bool setFromXML(AgentGenerator* gen, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  /*! The identifier for the "anchor_x" float parameter. */
  size_t _anchorXID;

  /*! The identifier for the "anchor_y" float parameter. */
  size_t _anchorYID;

  /*! The identifier for the "offset_x" float parameter. */
  size_t _offsetXID;

  /*! The identifier for the "offset_y" float parameter. */
  size_t _offsetYID;

  /*! The identifier for the "count_x" size_t parameter. */
  size_t _xCountID;

  /*! The identifier for the "count_y" size_t parameter. */
  size_t _yCountID;

  /*! The identifier for the "rotation" float parameter. */
  size_t _rotID;
};
}  // namespace Agents
}  // namespace Menge

#endif  // __RECT_GRID_GENERATOR_H__
