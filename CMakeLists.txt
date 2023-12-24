cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

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
        src/Data/Variable.h
        src/Data/Opcode.h

        src/ClassLoader/AttributeParser.h
        src/ClassLoader/AttributeParser.cpp
        src/ClassLoader/ClassLoader.h
        src/ClassLoader/ClassLoader.cpp
        src/ClassLoader/DescriptorParser.h
        src/ClassLoader/DescriptorParser.cpp
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
add_subdirectory(src/Library)
add_subdirectory(test)
add_subdirectory(tools/ClassDump)
