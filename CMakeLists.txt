cmake_minimum_required (VERSION 3.1 FATAL_ERROR)
project (bfio)

option(BFIO_EXAMPLES "Build bdio examples" OFF)
option(BFIO_INSTALL "Generate installation target" ON)
option(BFIO_TESTS "Build bdio tests" OFF)
option(BFIO_TEST_WITH_GLM "Build bdio tests for glm" OFF)

include_directories(include)

if (BFIO_EXAMPLES OR BFIO_TESTS)
	configure_file(thirdparty/CMakeLists.txt.in thirdparty/CMakeLists.txt)
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty")
	execute_process(COMMAND "${CMAKE_COMMAND}" --build .
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty" )
		
	include_directories(thirdparty/src/glm)
	include_directories(thirdparty/src/Catch/single_include)
endif()

if (BFIO_EXAMPLES)
	add_subdirectory(examples)
endif()

if (BFIO_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

if (BFIO_INSTALL)
	install(FILES include/bfio.h DESTINATION include)
	
	if (NOT TARGET uninstall)
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)

	add_custom_target(uninstall
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
		endif()
endif()
