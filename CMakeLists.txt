cmake_minimum_required(VERSION 2.8)
project(InstancingTest)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wextra -Wall -pedantic -Werror -Wno-unused-parameter")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-warn-absolute-paths")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths")
endif()

file(GLOB_RECURSE
     SRC_LIST
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     source/*.cpp source/*.h*)

file(GLOB_RECURSE
     ASSET_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     assets/*.frag assets/*.vert assets/*.glsl)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})

# Set output directories and copy assets to binary directory
set(ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
if(MSVC)
    message(STATUS "***** Using MSVC compiler *****")
    		
    file(GLOB DLLS DLLs/*)
    #set(DLLS ${CMAKE_CURRENT_SOURCE_DIR}/DLLs)
    
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4310 /wd4127 /wd4100")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4310 /wd4127 /wd4100")
    
    # Handle Visual Studio Debug/Release... config folders
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
        set(PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/Temp)
    endforeach()
    
    set(PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Temp)
    
    # Create source groups for Visual Studio filters
    # Get all directories first:
    set(dir_list "")
    foreach(file_path ${SRC_LIST} ${ASSET_FILES})
        get_filename_component(dir_path ${file_path} PATH)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list(REMOVE_DUPLICATES dir_list)
    
    file(GLOB sources *.cpp *.h*)
    source_group(\\ FILES ${sources})
    
    foreach(dir ${dir_list})
        set(abs_dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
        file(GLOB sources ${abs_dir}/*.cpp ${abs_dir}/*.h* ${abs_dir}/*.vert ${abs_dir}/*.frag ${abs_dir}/*.glsl)
        string(REPLACE "/" "\\\\" Group ${dir})
        source_group(${Group} FILES ${sources})
    endforeach()

    # Copy assets to the binary directory
    file(COPY ${ASSET_DIR} DESTINATION ${CMAKE_BINARY_DIR})
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        file(COPY ${ASSET_DIR} DESTINATION ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
        foreach(file_path ${DLLS})
            file(COPY ${file_path} DESTINATION ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG})
        endforeach()
    endforeach()
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    # Copy assets to the binary directory
    file(COPY ${ASSET_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

if(EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file assets")
endif()

add_subdirectory(third_party)
INCLUDE_DIRECTORIES(source)
INCLUDE_DIRECTORIES(third_party)

add_executable(${PROJECT_NAME} ${ASSET_FILES} ${SRC_LIST})

if(EMSCRIPTEN)
    message(STATUS "Using emscripten.")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_SDL=2 -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -s USE_SDL=2")
    set(GLEW_LIBRARIES "")
    set(OPENGL_LIBRARIES "")
    set(SDL2_LIBRARY "")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    find_package(SDL2 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    add_definitions(${OpenGL_DEFINITIONS})
    include_directories(${SDL2_INCLUDE_DIR} ${OpenGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR})
    link_directories(${OpenGL_LIBRARY_DIRS})
endif()

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
