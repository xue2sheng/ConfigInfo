cmake_minimum_required (VERSION 3.8)

project ("ConfigInfo")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
add_executable (${CMAKE_PROJECT_NAME} "main.cpp" "xmlInfo.cpp" "gui.cpp" "httpServer.cpp")
target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC pch.h)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /await)
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${CMAKE_PROJECT_NAME} WindowsApp HttpAPI Shlwapi)

add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.xml
    $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)