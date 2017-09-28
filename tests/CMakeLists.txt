file(GLOB HEADERS ../include/*.h)
include_directories(../externals/Catch/include)
include_directories(../externals/glm)

if (BFIO_TEST_WITH_GLM)
	add_definitions(-DBFIO_INCLUDE_GLM=1)
endif()

add_executable(tests tests.cpp ${HEADERS})

set_target_properties(tests PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS NO)

add_test(NAME CatchTests COMMAND tests)
