cmake_minimum_required(VERSION 3.1)
project(score_addon_quarre LANGUAGES CXX)

add_definitions(-DQUARRE_SERVER_NAME="quarre-server")
add_definitions(-DQUARRE_USERS_MAX=4)

score_common_setup()

set(QUARRE_SCORE_ADDON_HEADERS
    score_addon_quarre.hpp
    quarre/device/quarre-device.hpp
    quarre/device/quarre-protocol-factory.hpp
    quarre/device/quarre-protocol-settings-widget.hpp
    quarre/device/quarre-protocol-specific-settings.hpp
    )

set(QUARRE_SCORE_ADDON_IMPLEMENTATIONS
    score_addon_quarre.cpp
    quarre/device/quarre-device.cpp
    quarre/device/quarre-protocol-factory.cpp
    quarre/device/quarre-protocol-settings-widget.cpp
    quarre/device/quarre-protocol-specific-settings.cpp
    )

add_library(${PROJECT_NAME} ${QUARRE_SCORE_ADDON_HEADERS} ${QUARRE_SCORE_ADDON_IMPLEMENTATIONS})
score_generate_command_list_file(${PROJECT_NAME} "${QUARRE_SCORE_ADDON_HEADERS}")

target_link_libraries(${PROJECT_NAME} PUBLIC score_plugin_engine)
setup_score_plugin(${PROJECT_NAME})
