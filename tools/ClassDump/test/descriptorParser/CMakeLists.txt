



add_executable(descriptor_test 
main.cpp
${PROJECT_SOURCE_DIR}/src/ClassLoader/DescriptorParser.cpp
${PROJECT_SOURCE_DIR}/src/Util.h
${PROJECT_SOURCE_DIR}/src/Util.cpp
${PROJECT_SOURCE_DIR}/src/Platform.h
${PROJECT_SOURCE_DIR}/src/ByteArray.cpp
${PROJECT_SOURCE_DIR}/src/Platform.h
${PROJECT_SOURCE_DIR}/src/OutputBuffer.h
${PROJECT_SOURCE_DIR}/src/OutputBuffer.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_directories(descriptor_test PRIVATE Windows)
    target_link_libraries(descriptor_test Windows)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_directories(descriptor_test PRIVATE Linux)
    target_link_libraries(descriptor_test Linux)
endif()

target_include_directories(descriptor_test PUBLIC ${PROJECT_SOURCE_DIR}/src/)