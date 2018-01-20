file(GLOB HEADERS ../include/*.h)
add_executable(example_1 example_1.cpp ${HEADERS})
add_executable(example_2 example_2.cpp ${HEADERS})

file(COPY "archive.zip" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}")
