cmake_minimum_required( VERSION 3.22 )

project(Aktion LANGUAGES CXX)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY obj/ )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY archive/ )

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC
	src/*.h
	src/*.hpp
	src/*.cpp
)
	
source_group(TREE ../src/ FILES  ${SRC})

add_executable(Aktion ${SRC})

target_compile_features( ${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

target_sources( ${PROJECT_NAME}
    PRIVATE
		${SRC}
)

target_link_directories( ${PROJECT_NAME}
	PUBLIC
		"${CMAKE_SOURCE_DIR}/libs/SDL3-3.1.6/lib/x64"
		"${CMAKE_SOURCE_DIR}/libs/assimp/lib/x64"
		"${CMAKE_SOURCE_DIR}/libs/DevIL/lib/Release"
)

if( "${Platform}" STREQUAL "Windows" )
# Windows config
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE assimp-vc143-mt.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE DevIL.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ILU.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ILUT.lib)
else()
# MacOS or Linux config
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/SDL3-3.1.6/lib/x64/libSDL3.so)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/assimp/lib/x64/libassimp.so)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/DevIL/lib/Release/DevIL.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/DevIL/lib/Release/ILU.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/DevIL/lib/Release/ILUT.lib)
endif()

target_include_directories( ${PROJECT_NAME}
    PRIVATE
        "src"
		${Vulkan_INCLUDE_DIRS}
)

target_include_directories( ${PROJECT_NAME}
	PUBLIC
		"libs/SDL3-3.1.6/include"
		"libs/imgui-docking"
		"libs/assimp/include"
		"libs/DevIL/include/IL"
		"libs/imgui-node-editor-0.9.3"
		"libs/glm"
		"libs/json/include"
)

