# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(Dependencies)
SET(Dependencies 1.0)

if (OGREKIT_COMPILE_ENET)
	subdirs(enet)
endif()

if (OGREKIT_CREATE_OGRE_DEPENDENCY_DIR)
	set(OGRE_DEPENDENCIES_DIR ${CMAKE_BINARY_DIR}/PrebuiltDependencies)
	if (OGREKIT_BUILD_ANDROID)
		set(OGRE_PREBUILT_DEPENDENCY_LIB_DIR ${OGRE_DEPENDENCIES_DIR}/lib/${ANDROID_ABI})
	else()
		set(OGRE_PREBUILT_DEPENDENCY_LIB_DIR ${OGRE_DEPENDENCIES_DIR}/lib)
	endif()
	set(OGRE_PREBUILT_DEPENDENCY_INCLUDE_DIR ${OGRE_DEPENDENCIES_DIR}/include)
	
	file(MAKE_DIRECTORY ${OGRE_PREBUILT_DEPENDENCY_LIB_DIR})
	file(MAKE_DIRECTORY ${OGRE_PREBUILT_DEPENDENCY_INCLUDE_DIR})
endif()


subdirs(GameKit/Utils)
subdirs(GameKit/AnimKit)

if (OGREKIT_USE_BPARSE)
	include_directories(GameKit/Utils)
	subdirs(Blender25)
endif()

if (OGREKIT_HEADER_GENERATOR)
	subdirs(Blender25/Headers)
endif()

include(NoWarn)
subdirs(OIS)
subdirs(FreeType)

if (OGREKIT_COMPILE_OPENSTEER)
	subdirs(OpenSteer)
endif()

if (OGREKIT_COMPILE_RECAST)
	subdirs(Recast)
	subdirs(Detour)
endif()

if (OGREKIT_USE_STATIC_FREEIMAGE)
	subdirs(FreeImage)
endif()

if (NOT OGREKIT_DISABLE_ZIP)
	subdirs(ZZipLib)
endif()

if (OGREKIT_USE_LUA)
	subdirs(Lua)
endif()

if (OGREKIT_OPENAL_SOUND)
	subdirs(Codecs)
	if (WIN32)
		subdirs(OpenAL)
	endif()
endif()

if (OGREKIT_COMPILE_TINYXML)
    subdirs(TinyXml)
endif()
