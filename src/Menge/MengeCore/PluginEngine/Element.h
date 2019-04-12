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
 @file    Element.h
 Base class for all %Menge elements.
 */

#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "MengeCore/CoreConfig.h"

namespace Menge {

// TODO(curds01): Remove this after I've confirmed that I've fully decoupled visualization from
// simulation.
//// forward declarations
// namespace SceneGraph {
//  class GLNode;
//}

namespace BFSM {
class Task;
}

/*! The basic interface of extendible %Menge Elements.

 A %Menge element is a component of the %Menge framework. These elements can be combined at runtime
 to create a unique simulation, or even a unique simulator. Novel element implementations can be
 created in plugins and dynamically loaded. All such elements of %Menge ultimately derive from this
 base class. */
class MENGE_API Element {
 protected:
  virtual ~Element() {}

 public:
  /*! Destroys `this` element instance. The destructor is _not_ publicly available. Instead, they
    must be explicitly _destroyed_ via invocation of this method.

    This is largely an artficat of Windows dlls and C-runtime libraries. Each runtime library has
    its own heap. Depending on a how a dll is compiled, it may be allocated in a different heap than
    the thread that attempts to destroy it. The introduction of this method allows the derivec class
    free itself from its appropriate heap. */
  void destroy() { delete this; }

  /*! Return an optional task associated with this element.

   An element can have an accompanying Task for performing synchronized work. Most element
   implementations will not have a task.  If a task is required, override this function to return
   an appropriate instance of the corresponding task.

   @returns A pointer to the required task. If not nullptr, the caller takes ownership of the Task.
   */
  virtual BFSM::Task* getTask() { return 0x0; }

#if 0
  // TODO(curds01): Delete with the commented out forward declaration.
  /*!
    @brief    Returns an optional visualization element associated with the element.

    This element will simply be added to the scene graph and will be destroyed by the scene graph.
    It should not be used if the visualization is context dependent.

    @returns    A pointer to the scene graph node element.
    */
  virtual SceneGraph::GLNode * getSGNode() { return 0x0; }
#endif
};

}  // namespace Menge

#endif  // __ELEMENT_H__
