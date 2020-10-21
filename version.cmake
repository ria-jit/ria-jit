find_package(Git REQUIRED quiet)

execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --always
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

##Update this on Release so the fallback is right.
if (res)
    set(VERSION "v1.3.0")
endif ()

configure_file(${SRC} ${DST})
