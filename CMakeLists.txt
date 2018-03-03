cmake_minimum_required(VERSION 2.6)

project(vsrs)

# The version number.
set (vsrs_VERSION_MAJOR 4)
set (vsrs_VERSION_MINOR 3)

  

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug/Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})


if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

# macro to include directories recursively
MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

# remove ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# use a library
include_directories(
  ${PROJECT_SOURCE_DIR}/src/CommonLib
  ${PROJECT_SOURCE_DIR}/src/ViewSynLib
  ${PROJECT_SOURCE_DIR}/src/ViewSyn
)

# CommonLib
file(GLOB_RECURSE CommonLibSRC
    ${PROJECT_SOURCE_DIR}/src/CommonLib/*.c*
    ${PROJECT_SOURCE_DIR}/src/CommonLib/*.h*
	)
add_library(CommonLib  ${CommonLibSRC})
target_link_libraries(CommonLib ${OpenCV_LIBS})

# ViewSynthLib
file(GLOB_RECURSE ViewSynLibSRC
    ${PROJECT_SOURCE_DIR}/src/ViewSynLib/*.c*
    ${PROJECT_SOURCE_DIR}/src/ViewSynLib/*.h*
  )
add_library(ViewSynLib  ${ViewSynLibSRC})
target_link_libraries( ViewSynLib ${OpenCV_LIBS} CommonLib)

# ViewSyn
file(GLOB_RECURSE ViewSynSRC
    ${PROJECT_SOURCE_DIR}/src/ViewSyn/*.c*
	${PROJECT_SOURCE_DIR}/src/ViewSyn/*.h*
  )
add_executable(ViewSyn ${ViewSynSRC})
target_link_libraries( ViewSyn ${OpenCV_LIBS} CommonLib ViewSynLib)
#  cxcore cv highgui cvcam




