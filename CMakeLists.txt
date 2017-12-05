cmake_minimum_required(VERSION 3.1)
project(score_addon_quarre LANGUAGES CXX)

score_common_setup()

set(QUARRE_SCORE_ADDON_HEADERS
    score_addon_quarre.hpp)

set(QUARRE_SCORE_ADDON_IMPLEMENTATIONS
    score_addon_quarre.cpp)

add_library(${PROJECT_NAME} ${QUARRE_SCORE_ADDON_HEADERS} ${QUARRE_SCORE_ADDON_IMPLEMENTATIONS})
score_generate_command_list_file(${PROJECT_NAME} "${QUARRE_SCORE_ADDON_HEADERS}")

target_link_libraries(${PROJECT_NAME} PUBLIC score_plugin_engine)
setup_score_plugin(${PROJECT_NAME})
