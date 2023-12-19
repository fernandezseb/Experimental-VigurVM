
add_library(WindowsClassDump Platform.cpp Memory.cpp)
target_include_directories(WindowsClassDump PUBLIC ${PROJECT_SOURCE_DIR}/src/)
