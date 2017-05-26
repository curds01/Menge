/*!
 *	@file		PowerLaw.cpp  
 *	@brief		Plugin for helbing pedestrian.
 */

#include "UplConfig.h"
#include "UplAgentContext.h"
#include "UplDBEntry.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "MengeVis/PluginEngine/VisPluginEngine.h"

using Menge::PluginEngine::CorePluginEngine;
using MengeVis::PluginEngine::VisPluginEngine;

extern "C" {
  /*!
   *	@brief		Retrieves the name of the plug-in.
   *
   *	@returns	The name of the plug in.
   */
  POWERLAW_API const char * getName() {
	return "Universal Power Law Pedestrian Model";
  }

  /*!
   *	@brief		Description of the plug-in.
   *
   *	@returns	A description of the plugin.
   */
  POWERLAW_API const char * getDescription() {
	return	"A pedestran plugin based on the model proposed in 2014 by Karamouzas et al.";
  }

  /*!
   *	@brief		Registers the plug-in with the PluginEngine
   *
   *	@param		engine		A pointer to the plugin engine.
   */
  POWERLAW_API void registerCorePlugin( CorePluginEngine * engine ) {
    engine->registerModelDBEntry( new PowerLaw::DBEntry() );
  }

  /*!
   *	@brief		Registers the plug-in with the visualization plug-in engine.
   *
   *	@param		engine		A pointer to the vis engine.
   */
  POWERLAW_API void registerVisPlugin( VisPluginEngine * engine ) {
    engine->registerAgentContext( new PowerLaw::AgentContext() );
  }

}
