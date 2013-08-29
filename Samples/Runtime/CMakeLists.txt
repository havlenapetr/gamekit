# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

if (APPLE)
	include_directories(${OGRELITE_SOURCE_DIR}/OgreMain/include/OSX)
	SET(EXETYPE MACOSX_BUNDLE)
	SET_SOURCE_FILES_PROPERTIES( MainMenu.nib  PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES(momo_ogre.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET (NibFile MainMenu.nib)
endif()

if (OGREKIT_BUILD_ANDROID)
	include_directories(
		${ANDROID_NDK}/sources/android/native_app_glue
	)
	set(SRC 
		${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c
		${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
		android.cpp
	)
	link_libraries(
		android
		log
	)
else()
	include_directories(
		${OGREKIT_DEP_DIR}/tclap/include
	)
	set(SRC 
		momo_ogre.blend
		Main.cpp
	)
endif()

include_directories(
	${OGREKIT_INCLUDE}
)

link_libraries(
	${OGREKIT_LIB}
)

set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})

if (WIN32)
	set(PLAT_RESOURCE Win32/app.rc)
	set(RESOURCE )	
	set_source_files_properties(${RESOURCE} PROPERTIES HEADER_FILE_ONLY 1)
	source_group(Content\\Resources FILES ${PLAT_RESOURCE} ${RESOURCE})

	list(APPEND SRC ${PLAT_RESOURCE})
endif()

if (APPLE)
	add_executable(
		AppOgreKit 
		MACOSX_BUNDLE
		MainMenu.nib
		${SRC} 
		${HiddenCMakeLists}
	)
else ()
	if (OGREKIT_BUILD_ANDROID)
		add_library(Runtime SHARED ${SRC} ${HiddenCMakeLists})
	else()
		add_executable(AppOgreKit ${SRC} ${HiddenCMakeLists})

		IF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
			ADD_CUSTOM_COMMAND(
				TARGET AppOgreKit 
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/momo_ogre.blend ${CMAKE_CURRENT_BINARY_DIR}/momo_ogre.blend
			)
		ENDIF ()
	endif()
endif()
