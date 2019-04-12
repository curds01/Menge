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
 @file    ElementFactory.h
 The base (templated) implementation of a factory for a plug-in element.
 */

#ifndef __ELEMENT_FACTORY_H__
#define __ELEMENT_FACTORY_H__

#include "MengeCore/PluginEngine/AttributeSet.h"

// forward declaration
class TiXmlElement;

namespace Menge {

/*! To support the plugin archtitecture, when a plug-in creates a new implementaiton of an Element
 type, it registerse a _factory_ to %Menge. The factory is responsible for consuming XML and
 instantiating an instance of its Element type. 
 
 This base class provides the common basis for all Element types, reducing the amount of code for
 derived classes to implement.

 @tparam Element  The type of element this factory produces. %Menge uses this template parameters
 to specialize the base %ElementFactory into the base class factory for each supported element
 type. Plugin code will generally not see this template parameter and will sub-class directly
 from the particularly element's factory class. */
template <class Element>
class ElementFactory {
 public:
  ElementFactory() {}

  /*! Destroys `this` factory instance. The destructor is _not_ publicly available. Instead, they
   must be explicitly _destroyed_ via invocation of this method.

   This is largely an artficat of Windows dlls and C-runtime libraries. Each runtime library has its
   own heap. Depending on a how a dll is compiled, it may be allocated in a different heap than
   the thread that attempts to destroy it. The introduction of this method allows the derivec class
   free itself from its appropriate heap. */
  void destroy() { delete this; }

 protected:
  virtual ~ElementFactory() {}

 public:
  /*! Reports the name of the element type as mainfest in the XML code. Eaach element type has a
   corresponding XML tag (e.g., `AgentGeneroator` relate to the `<AgentGenerator>` tag. That tag
   will have a property: `type` whose value must match the `name()` of one and only one registered
   factory for that element type.

   The `name()` reported for `this` factory must be unique among all factories for the same element
   type. Duplicate names will lead to logged reigstration errors (see
   ElementDatabase::addFactory()).

   @returns  A string containing the unique action name. */
  virtual const char* name() const = 0;

  /*! Provides a description of this factory's element type.

   Each element should provide a brief description for the element. This is used for runtime
   introspection of the dynamically loaded elements. */
  virtual const char* description() const = 0;

  /*! Reports if the `type` parameter refers to the element type that `this` factory creates.
   As indicted earlier, each element type has a defined XML tag and will always have a `type` 
   parameter. E.g.,:
   
   @code{xml}
   <SomeElement type="bob" param1="10" .../>
   @endcode

   Once the `<SomeElement>` tag has been matched to the right ElementDatabase, the value of the
   `type` property ("bob" in the above example), will be passed to each registered factory. It
   should always be the case that this method returns true whenever the type parameter `type_name`
   matches the result from `name()`. */
  bool thisFactory(const std::string& type_name) const { return type_name == name(); }

  /*! Creates an instance of this factory's element from the given XML data.
  
   It creates the element instance by invoking the `instance()` method. That instance is passed into
   the setFromXML() method. If setFromXML() reports failure, then the created instnace is destroyed
   and nullptr is returned.

   @param   node        The xml node containing the data for the element.
   @param   xml_folder  The path to the folder containing the XML file. If the element references
                        resources in the file system, it should be defined relative to the xml file
                        location. This path allows for turning XML-relative paths into absolute 
                        paths.
   @returns A pointer to a new element instance for this XML node. If there is an error in the xml,
            nullptr is returned. */
  Element* createInstance(TiXmlElement* node, const std::string& xml_folder) const {
    Element* element = instance();
    if (!setFromXML(element, node, xml_folder)) {
      element->destroy();
      element = 0x0;
    }
    return element;
  }

 protected:
  /*! Create an instance of this class's element type. This is used to create the instance in
   createInstance(). It eliminates the strict need that the element type must be default
   constructible. The factory should create the element instance on the heap. The caller of this
   function will take ownership of the instance. */
  virtual Element* instance() const = 0;

  /*! Sets the fields in the given element `instance` based on the data stored in the given XML
   `node`.

   It is assumed that the value of the XML `node`'s `type` attribute will be the type created by
   `this` factory (i.e. ElementFactory::thisFactory() has already been called and returned true). If
   sub-classes of %ElementFactory introduce _new_ parameters, then the sub-class should override
   this method but explicitly call the parent class's version.

   @todo Replace this method with an NVI-based setup to eliminate the need of a derived class
         invoking the parent class's implementation.

   @param   element     A pointer to the element type whose attributes are to be set. It should be
                        dynamically-castable to this factory's element type.
   @param   node        The XML node containing the element attributes.
   @param   xml_folder  The path to the folder containing the XML file. If any of the data in `node`
                        references resources in the file system, it should be defined relative to
                        the xml file location. This path allows for turning XML-relative paths into
                        absolute paths. If the element type does not require file-system resources,
                        this parameter can be ignored.
   @returns True if the `element` was successfully configured from the XML `node`.
   @throws  An AttributeException if there is a problem in extracting values from the attribute set.
   */
  virtual bool setFromXML(Element* element, TiXmlElement* node,
                          const std::string& xml_folder) const {
    _attrSet.clear();
    return _attrSet.extract(node);
  }

  /*! The attribute set for this factory. The factory constructor should declare attributes to
   facilitate parsing. */
  mutable AttributeSet _attrSet;
};
}  // namespace Menge

#endif  // __ELEMENT_FACTORY_H__
