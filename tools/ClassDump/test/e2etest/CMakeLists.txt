find_package(Java REQUIRED)
include(UseJava)


project (e2etest NONE)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

add_jar(e2etest
	Main.java
	ConstantValue.java
	Exceptions.java
	Unicode.java
)

#get_target_property(_jarFile e2etest JAR_FILE)
get_target_property(_classDir e2etest CLASSDIR)

add_test(NAME e2etest COMMAND ClassDump "${_classDir}/Main.class" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME e2etestConstantValue COMMAND ClassDump "${_classDir}/ConstantValue.class" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME e2etestExceptions COMMAND ClassDump "${_classDir}/Exceptions.class" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME e2etestUnicode COMMAND ClassDump "${_classDir}/Unicode.class" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set (failRegex "^Error:")
set_property(TEST e2etest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")