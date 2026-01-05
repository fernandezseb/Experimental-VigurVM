
if (WIN32)
    file(COPY windows/rt.jar
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/rt.jar)
    file(COPY windows/charsets.jar
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/charsets.jar)
endif (WIN32)

if(LINUX)
    file(COPY linux/rt.jar
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/rt.jar)
    file(COPY linux/charsets.jar
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/charsets.jar)
endif()