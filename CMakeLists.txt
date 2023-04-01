cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(join_server VERSION ${PROJECT_VESRION})

configure_file(version.h.in version.h)

find_package(Boost
  COMPONENTS
  program_options)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
  server.h
  server.cxx
  data_base.h
  data_base.cxx
  task_processor.h
)

add_executable(client
  data_base.h
  data_base.cxx
  session.h
  session.cxx
  task_processor.h
)

option(CHECK_DOXYGEN "enable doc generation" ON)

if (${CHECK_DOXYGEN} STREQUAL ON)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYFILE ${CMAKE_SOURCE_DIR}/Doxyfile)
        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen")
        unset(DOXYFILE)
    endif()
endif()

target_link_libraries(server
    ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(server
  PRIVATE "${CMAKE_BINARY_DIR}")

target_include_directories(server
  PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(server
  PRIVATE Boost::boost)

target_include_directories(client
  PRIVATE "${CMAKE_BINARY_DIR}")

target_include_directories(client
  PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(client
  PRIVATE Boost::boost
  PRIVATE Boost::program_options
  )

add_executable(tests
  data_base.h
  data_base.cxx
  data_base_test.cxx)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1 # нужный релиз
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

install(TARGETS server RUNTIME DESTINATION bin)
install(TARGETS client RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

enable_testing()
add_test(tests tests)
