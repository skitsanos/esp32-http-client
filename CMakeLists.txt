cmake_minimum_required(VERSION 3.5)

message("CMAKE version: " ${CMAKE_VERSION})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp32http-client)

set_target_properties(idf_component_main PROPERTIES LINKER_LANGUAGE C)
set_target_properties(idf_component_uri_parser PROPERTIES LINKER_LANGUAGE C)
set_target_properties(idf_component_esp_request PROPERTIES LINKER_LANGUAGE C)

set_target_properties(idf_component_internal PROPERTIES LINKER_LANGUAGE C)
set_target_properties(idf_component_utils PROPERTIES LINKER_LANGUAGE C)
set_target_properties(idf_component_wifi PROPERTIES LINKER_LANGUAGE C)