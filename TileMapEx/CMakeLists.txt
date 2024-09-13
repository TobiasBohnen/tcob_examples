add_executable(TileMapEx)

target_sources(TileMapEx PRIVATE
    main.cpp
    TileMapEx.cpp
)

set_target_properties(TileMapEx PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(EMSCRIPTEN)
    target_compile_options(TileMapEx PRIVATE -fexperimental-library -pthread)
    configure_file(tilemapex.zip ${CMAKE_BINARY_DIR}/tilemapex.zip COPYONLY)
    target_link_options(TileMapEx PRIVATE -sFULL_ES3 -sASSERTIONS -sPTHREAD_POOL_SIZE=8 -sASYNCIFY -pthread --embed-file tilemapex.zip)
else()
    configure_file(tilemapex.zip ${CMAKE_BINARY_DIR}/../../x64/tilemapex.zip COPYONLY)
endif()

if(NOT TCOB_BUILD_SHARED)
    target_link_libraries(TileMapEx PRIVATE tcob_static)
else()
    target_link_libraries(TileMapEx PRIVATE tcob_shared)
endif()

target_include_directories(TileMapEx PRIVATE ../../../tcob/include)

if(TCOB_BUILD_SHARED AND NOT TCOB_IS_CI)
    add_custom_command(TARGET TileMapEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:TileMapEx> ${CMAKE_BINARY_DIR}/../../x64/
        COMMAND_EXPAND_LISTS
    )
endif()
