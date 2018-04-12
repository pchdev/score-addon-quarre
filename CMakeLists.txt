cmake_minimum_required(VERSION 3.1)
project(score_addon_quarre LANGUAGES CXX)

score_common_setup()

set(QUARRE_SCORE_ADDON_HEADERS
    score_addon_quarre.hpp
    quarre/application-plugin/quarre-application-plugin.hpp
    quarre/commands/quarre-command-factory.hpp
    quarre/device/quarre-device.hpp
    quarre/device/quarre-protocol-factory.hpp
    quarre/device/quarre-protocol-settings-widget.hpp
    quarre/device/quarre-protocol-specific-settings.hpp
    quarre/process/quarre-process-factory.hpp
    quarre/process/quarre-process-metadata.hpp
    quarre/process/quarre-process-model.hpp
    quarre/process/inspector/quarre-process-inspector.hpp
    quarre/process/layer/quarre-process-layer-factory.hpp
    quarre/process/layer/quarre-process-layer-presenter.hpp
    quarre/process/layer/quarre-process-layer-view.hpp
    quarre/process/local-tree/quarre-process-local-tree.hpp
    quarre/process/executor/quarre-process-executor.hpp
    quarre/interaction/quarre-interaction.hpp
    quarre/panel/quarre-panel-delegate.hpp
    quarre/panel/quarre-panel-delegate-factory.hpp
    quarre/mapping/quarre-mapping.hpp
    )

set(QUARRE_SCORE_ADDON_IMPLEMENTATIONS
    score_addon_quarre.cpp
    quarre/application-plugin/quarre-application-plugin.cpp
    quarre/device/quarre-device.cpp
    quarre/device/quarre-protocol-factory.cpp
    quarre/device/quarre-protocol-settings-widget.cpp
    quarre/device/quarre-protocol-specific-settings.cpp
    quarre/process/quarre-process-model.cpp
    quarre/process/inspector/quarre-process-inspector.cpp
    quarre/process/layer/quarre-process-layer-presenter.cpp
    quarre/process/layer/quarre-process-layer-view.cpp
    quarre/process/local-tree/quarre-process-local-tree.cpp
    quarre/process/executor/quarre-process-executor.cpp
    quarre/interaction/quarre-interaction.cpp
    quarre/panel/quarre-panel-delegate.cpp
    quarre/mapping/quarre-mapping.cpp
    )

add_library(${PROJECT_NAME} ${QUARRE_SCORE_ADDON_HEADERS} ${QUARRE_SCORE_ADDON_IMPLEMENTATIONS})
score_generate_command_list_file(${PROJECT_NAME} "${QUARRE_SCORE_ADDON_HEADERS}")

target_link_libraries(${PROJECT_NAME} PUBLIC score_plugin_engine)
setup_score_plugin(${PROJECT_NAME})
