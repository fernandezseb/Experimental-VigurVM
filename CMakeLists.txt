cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

cmake_policy(SET CMP0077 NEW)

project(VigurVM
        VERSION 0.1
        DESCRIPTION "A simple Java Virtual Machine for learning purposes"
        LANGUAGES CXX
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Core.h
        src/ByteArray.h
        src/ByteArray.cpp
        src/Platform.h
        src/CoreUtils.cpp
        src/CoreUtils.h
        src/StringUtils.h
        src/StringUtils.cpp
        src/Memory.h
        src/Error.h
        src/DynamicArray.cpp
        src/DynamicArray.h
        src/VM/VM.cpp
        src/VM/VM.h
        src/VM/Configuration.h
        src/VM/JavaHeap.h
        src/VM/JavaHeap.cpp
        src/VM/VMThread.h
        src/VM/JavaStack.h
        src/VM/VMThread.cpp
        src/VM/Native.h
        src/VM/Native.cpp

        src/Data/Attribute.h
        src/Data/Attribute.cpp
        src/Data/Class.h
        src/Data/ConstantPool.h
        src/Data/ConstantPool.cpp
        src/Data/Descriptor.h
        src/Data/VData.h
        src/Data/VData.cpp
        src/Data/Opcode.h

        src/ClassLoader/AttributeParser.h
        src/ClassLoader/AttributeParser.cpp
        src/ClassLoader/ClassLoader.h
        src/ClassLoader/ClassLoader.cpp
        src/ClassLoader/DescriptorParser.h
        src/ClassLoader/DescriptorParser.cpp
        src/VM/Instruction.h
        src/VM/Instructions/ConstantInstructions.h
        src/VM/Instructions/ConstantInstructions.cpp
        src/VM/Instructions/LoadInstructions.h
        src/VM/Instructions/LoadInstructions.cpp
        src/VM/Instructions/StoreInstructions.h
        src/VM/Instructions/StoreInstructions.cpp
        src/VM/Instructions/StackInstructions.h
        src/VM/Instructions/StackInstructions.cpp
        src/VM/Instructions/ReferenceInstructions.h
        src/VM/Instructions/ReferenceInstructions.cpp
        src/VM/Instructions/ControlInstructions.h
        src/VM/Instructions/ControlInstructions.cpp
        src/VM/Instructions/ExtendedInstructions.h
        src/VM/Instructions/ExtendedInstructions.cpp
        src/VM/Instructions/MathInstructions.h
        src/VM/Instructions/MathInstructions.cpp
        src/VM/Instructions/ComparisonInstructions.h
        src/VM/Instructions/ComparisonInstructions.cpp
        src/VM/Instructions/ConversionInstructions.h
        src/VM/Instructions/ConversionInstructions.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} /GR- -EHa- -Gm- -DUNICODE -D_UNICODE -W4")

    add_subdirectory(src/Windows)
    target_link_directories(${PROJECT_NAME} PRIVATE Windows)
    target_link_libraries(${PROJECT_NAME} Windows)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(src/Linux)
    target_link_directories(${PROJECT_NAME} PRIVATE Linux)
    target_link_libraries(${PROJECT_NAME} Linux)
endif()

target_link_directories(${PROJECT_NAME} INTERFACE Library)
target_link_libraries(${PROJECT_NAME} Library)

enable_testing()

add_subdirectory(extern/md5)
SET(PHYSFS_BUILD_TEST OFF CACHE BOOL "Build stdio test program." FORCE)
SET(PHYSFS_BUILD_SHARED OFF CACHE BOOL "Build shared library" FORCE)
SET(PHYSFS_BUILD_DOCS OFF CACHE BOOL "Build doxygen based documentation" FORCE)
add_subdirectory(extern/physfs)
add_subdirectory(extern/classpath)
add_subdirectory(src/Library)
add_subdirectory(test)
add_subdirectory(tools/ClassDump)

target_link_directories(${PROJECT_NAME} PRIVATE extern/physfs/src)
target_link_libraries(${PROJECT_NAME} physfs-static)