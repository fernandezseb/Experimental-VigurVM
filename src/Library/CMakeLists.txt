add_library(Library
        NativeDefs.h
        Builtin.h
        Builtin.cpp
        Vigur/lang/System.h
        Vigur/lang/System.cpp
        java/lang/Object.h
        java/lang/Object.cpp
        java/lang/System.h
        java/lang/System.cpp)
target_include_directories(Library PRIVATE ${PROJECT_SOURCE_DIR}/src/)