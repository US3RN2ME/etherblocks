file(GLOB_RECURSE ETHERBLOCKS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_library(etherblocks)

target_sources(etherblocks PRIVATE ${ETHERBLOCKS_SOURCES})

target_compile_features(etherblocks PUBLIC cxx_std_23)

target_link_libraries(etherblocks PUBLIC glfw glad::glad glm::glm)

target_include_directories(etherblocks PUBLIC ${Stb_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)
