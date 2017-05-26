cmake_minimum_required(VERSION 2.8)

project(UplModel)

add_definitions( -DPOWERLAW_EXPORT )

file(
	GLOB_RECURSE
	source_files
	${PLUGIN_SOURCE_DIR}/AgtUPL/*.cpp
	${PLUGIN_SOURCE_DIR}/AgtUPL/*.h
)

add_library(
	uplPed
	SHARED
	${source_files}
)

target_link_libraries (uplPed mengeCore mengeVis )
