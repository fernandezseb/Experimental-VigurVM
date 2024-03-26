find_package(Java REQUIRED)
include(UseJava)


project (e2e NONE)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

add_jar(e2e
	TestValidationException.java
	SmokeTest.java
	PrivilegedTest.java
	InstanceofTest.java
	LibraryTest.java
	ArgsTest.java
	StaticTest.java
	LookupSwitchTest.java
)

get_target_property(_classDir e2e CLASSDIR)

add_test(NAME SmokeTest COMMAND VigurVM "-classpath" "${_classDir}" "SmokeTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME PrivilegedTest COMMAND VigurVM "-classpath" "${_classDir}" "PrivilegedTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME InstanceofTest COMMAND VigurVM "-classpath" "${_classDir}" "InstanceofTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME LibraryTest COMMAND VigurVM "-classpath" "${_classDir}" "LibraryTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME ArgsTest COMMAND VigurVM "-classpath" "${_classDir}" "ArgsTest" "arg1" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME StaticTest COMMAND VigurVM "-classpath" "${_classDir}" "StaticTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME LookupSwitchTest COMMAND VigurVM "-classpath" "${_classDir}" "LookupSwitchTest" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

set (failRegex "^Error:")
set_property(TEST SmokeTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST PrivilegedTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST InstanceofTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST LibraryTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST ArgsTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST StaticTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")
set_property(TEST LookupSwitchTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")