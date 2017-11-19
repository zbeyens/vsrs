cmake_minimum_required(VERSION 2.6)

project(vsrs)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug/Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package( OpenCV REQUIRED )


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(
  ./CommonLibStatic/include
  ./ViewSynLibStatic/include
  ./ViewSyn/include
)

# CommonLibStatic
file(GLOB CommonLibStaticSRC
    ./CommonLibStatic/src/ParameterBase.cpp
    ./CommonLibStatic/src/upsample.cpp
    ./CommonLibStatic/src/yuv.cpp
	)
add_library(CommonLibStatic STATIC ${CommonLibStaticSRC})
target_link_libraries(CommonLibStatic ${OpenCV_LIBS})

# ViewSynthLibStatic
file(GLOB ViewSynLibStaticSRC
    ./ViewSynLibStatic/src/BoundaryNoiseRemoval.cpp
    ./ViewSynLibStatic/src/ParameterViewInterpolation.cpp
    ./ViewSynLibStatic/src/ViewInterpolation.cpp
    ./ViewSynLibStatic/src/ViewSynthesis.cpp
    ./ViewSynLibStatic/src/Visbd.cpp
  )
add_library(ViewSynLibStatic STATIC ${ViewSynLibStaticSRC})
target_link_libraries( ViewSynLibStatic ${OpenCV_LIBS} CommonLibStatic)

# ViewSyn
file(GLOB ViewSynSRC
    ./ViewSyn/src/ViewSyn.cpp
  )
add_executable(ViewSyn ${ViewSynSRC})
target_link_libraries( ViewSyn ${OpenCV_LIBS} CommonLibStatic ViewSynLibStatic)
#  cxcore cv highgui cvcam




