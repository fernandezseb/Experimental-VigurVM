add_executable(DynamicArrayTest
        main.cpp
        ${PROJECT_SOURCE_DIR}/src/Platform.h
        ${PROJECT_SOURCE_DIR}/src/DynamicArray.h
        ${PROJECT_SOURCE_DIR}/src/DynamicArray.cpp
        ${PROJECT_SOURCE_DIR}/src/OutputBuffer.h
        ${PROJECT_SOURCE_DIR}/src/OutputBuffer.cpp
        ${PROJECT_SOURCE_DIR}/src/Util.h
        ${PROJECT_SOURCE_DIR}/src/Util.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_directories(DynamicArrayTest PRIVATE Windows)
    target_link_libraries(DynamicArrayTest Windows)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_directories(DynamicArrayTest PRIVATE Linux)
    target_link_libraries(DynamicArrayTest Linux)
endif()

target_include_directories(DynamicArrayTest PUBLIC ${PROJECT_SOURCE_DIR}/src/)