if(TCOB_ENABLE_ADDON_PHYSICS_BOX2D
    AND TCOB_ENABLE_ADDON_AUDIO_TINYSOUNDFONT
    AND TCOB_ENABLE_ADDON_SCRIPTING_LUA
    AND TCOB_ENABLE_ADDON_GFX_LITEHTML)
    add_subdirectory(TestGame)

    add_subdirectory(CanvasEx)
    add_subdirectory(ControllerEx)
    add_subdirectory(PhysicsEx)
    add_subdirectory(SoundGeneratorEx)
    add_subdirectory(TextEx)
    add_subdirectory(TweenEx)
    add_subdirectory(UIEx)
endif()
