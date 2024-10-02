cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "2" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.1.${PATCH_VERSION})

project(mapper_reducer_prg VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS unit_test_framework  thread REQUIRED)
#lboost_system
# Библиотека libasync
add_library(async SHARED libasync/async.cpp)
#add_library(async STATIC libasync/async.cpp)
# Установка библиотеки libasync
install(TARGETS async DESTINATION /usr/local/lib)
install(FILES async.h DESTINATION /usr/local/include)

add_executable(bulk_server main.cpp
    server.cpp
    server.h)

#add_executable(bulk main.cpp
#    libasync/libasync.cpp
#    async.h)

add_executable(test_bulk test_bulk.cpp)

add_executable(mapper_av average/mapper.cpp produser_consumer.h mapper_all.h)
add_executable(mapper_ds dispersion/mapper.cpp produser_consumer.h mapper_all.h)
add_executable(reducer_av average/reducer.cpp)
add_executable(reducer_ds dispersion/reducer.cpp)
add_executable(test_average average/test_average.cpp)
add_executable(test_dispersion dispersion/test_dispersion.cpp)


set_target_properties(bulk_server test_bulk async mapper_av mapper_ds reducer_av reducer_ds test_average test_dispersion PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
#target_include_directories(bulk_prg
#    PRIVATE "${CMAKE_BINARY_DIR}"
#)

target_link_libraries(mapper_av
    ${Boost_LIBRARIES}
)
target_link_libraries(mapper_ds
    ${Boost_LIBRARIES}
)
target_link_libraries(reducer_av
    ${Boost_LIBRARIES}
)
target_link_libraries(reducer_ds
    ${Boost_LIBRARIES}
)



set_target_properties(test_bulk  test_average test_dispersion PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(bulk_server async)
#target_link_libraries(bulk_server Boost::thread)
target_link_libraries(bulk_server
    ${Boost_LIBRARIES}
    #bulk_prg
)

#install(TARGETS main DESTINATION bin)
#install(TARGETS bulk DESTINATION /usr/local/bin)

target_link_libraries(test_bulk
    ${Boost_LIBRARIES}
    #bulk_prg
)
target_link_libraries(test_average ${Boost_LIBRARIES} )
target_link_libraries(test_dispersion ${Boost_LIBRARIES} )

if (MSVC)
    target_compile_options(bulk_server PRIVATE
        /W4
    )
    target_compile_options(bulk_prg PRIVATE
        /W4
    )
    target_compile_options(test_bulk PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk_server PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
#    target_compile_options(bulk_prg PRIVATE
#        -Wall -Wextra -pedantic -Werror
#    )
    target_compile_options(test_bulk PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    #target_compile_options(mapper_av PRIVATE
       # -Wall -Wextra -pedantic #-Werror
    #)
    target_compile_options(mapper_ds PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(reducer_av PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(reducer_ds PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_average PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_dispersion PRIVATE
        -Wall -Wextra -pedantic -Werror
    )

endif()



install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_bulk test_bulk)
add_test(test_average test_average)
add_test(test_dispersion test_dispersion)
