add_library(LinuxClassDump Platform.cpp Memory.cpp)
target_include_directories(LinuxClassDump PUBLIC ${PROJECT_SOURCE_DIR}/src/)
