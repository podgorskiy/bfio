file(GLOB HEADERS ../include/*.h)
add_executable(example_1 example_1.cpp ${HEADERS})
add_executable(example_2 example_2.cpp ${HEADERS})

set_target_properties(example_1 PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS NO)
set_target_properties(example_2 PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS NO)

file(COPY "archive.zip" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}")
