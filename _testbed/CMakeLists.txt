add_executable(TestGame)

target_sources(TestGame PRIVATE
    main.cpp
    Misc.cpp
)

set_target_properties(TestGame PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
)

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(TestGame PRIVATE tcob_static)
else()
    target_link_libraries(TestGame PRIVATE tcob_shared)
endif()

if(MSVC)
    target_link_options(TestGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/icon.res)
endif()

target_include_directories(TestGame PRIVATE ../../../tcob/include)
