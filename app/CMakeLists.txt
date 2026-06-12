file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(app)

target_sources(app PRIVATE ${APP_SOURCES})

target_compile_features(app PUBLIC cxx_std_23)

target_link_libraries(app PRIVATE etherblocks)

add_custom_command(
   TARGET app
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:app>/shaders"
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:app>/assets")
