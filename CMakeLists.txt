cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(VigurVM
        VERSION 0.1
        DESCRIPTION "A simple Java Virtual Machine for learning purposes"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Core.h
        src/ByteArray.h
        src/ByteArray.cpp
        src/Platform.h
        src/Util.cpp
        src/Util.h
        src/Memory.h
        src/OutputBuffer.h
        src/OutputBuffer.cpp
        src/DynamicArray.cpp
        src/DynamicArray.h
        src/VM/VM.cpp
        src/VM/VM.h
        src/VM/Configuration.h

        src/Data/Attribute.h
        src/Data/Attribute.cpp
        src/Data/Class.h
        src/Data/ConstantPool.h
        src/Data/ConstantPool.cpp
        src/Data/Descriptor.h
        src/Data/Variable.h

        src/ClassLoader/AttributeParser.h
        src/ClassLoader/AttributeParser.cpp
        src/ClassLoader/ClassLoader.h
        src/ClassLoader/ClassLoader.cpp
        src/ClassLoader/DescriptorParser.h
        src/ClassLoader/DescriptorParser.cpp


)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} /GR- -EHa- -Gm- -DUNICODE -D_UNICODE")

    add_subdirectory(src/Windows)
    target_link_directories(${PROJECT_NAME} PRIVATE Windows)
    target_link_libraries(${PROJECT_NAME} Windows)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(src/Linux)
    target_link_directories(${PROJECT_NAME} PRIVATE Linux)
    target_link_libraries(${PROJECT_NAME} Linux)
endif()

enable_testing()
add_subdirectory(test)