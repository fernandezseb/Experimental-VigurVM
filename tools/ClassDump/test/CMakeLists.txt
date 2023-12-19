
add_subdirectory(descriptorParser)
add_subdirectory(e2etest)

add_test(NAME DescriptorTests COMMAND descriptor_test)

add_test(NAME VersionTest COMMAND ClassDump "-v" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set (failRegex "^Error:")
set (passRegex "^ClassDump version 0.1")
set_property(TEST VersionTest PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")
set_property(TEST VersionTest PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")