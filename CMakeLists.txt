cmake_minimum_required(VERSION 3.14)

if (WIN32)
    project(Cliptimizer LANGUAGES CXX)
elseif(UNIX)
    project(Cliptimizer)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)


#=================== INCLUSION OF Project Files ====================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/forms")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${FORMS_DIR}/*.ui"
    "${FORMS_DIR}/*.qrc"
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

#=================== SETUP EXECTUABLE ====================#
# Enable debug logging on RELWITHDEBINFO configuration
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Cliptimizer.rc")

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Add the executable
if (WIN32) 
    add_executable(Cliptimizer WIN32 ${SOURCES} ${app_icon_resource_windows})
elseif(UNIX)
    add_executable(Cliptimizer ${SOURCES} ${app_icon_resource_windows})
endif()

# Add the target includes for Cliptimizer 
target_include_directories(Cliptimizer PRIVATE ${FORMS_DIR})
target_include_directories(Cliptimizer PRIVATE ${INCLUDE_DIR})
target_include_directories(Cliptimizer PRIVATE ${SOURCE_DIR})
target_precompile_headers(Cliptimizer PUBLIC stdafx.h)

#===================== LINKING LIBRARIES =======================#
target_link_libraries(Cliptimizer Qt6::Widgets)

target_link_libraries(Cliptimizer Qt6::MultimediaWidgets)
target_link_libraries(Cliptimizer Qt6::Multimedia)
