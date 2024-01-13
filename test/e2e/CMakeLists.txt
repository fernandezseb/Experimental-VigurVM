find_package(Java REQUIRED)
include(UseJava)


project (e2e NONE)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

add_jar(e2e
	SmokeTest.java
)

get_target_property(_classDir e2e CLASSDIR)

add_test(NAME SmokeTest COMMAND VigurVM "-classpath" "${_classDir}" "SmokeTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

set (failRegex "^Error:")
set_property(TEST SmokeTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")