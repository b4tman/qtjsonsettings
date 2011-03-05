cmake_minimum_required(VERSION 2.6)

set(CMAKE_DEBUG_POSTFIX _debug)

# Declare project
project(qtjsonsettings)

find_package(Qt4 REQUIRED)
set(QT_DONTUSE_QTGUI TRUE)

include(${QT_USE_FILE})

message("Building project ${PROJECT_NAME}")

message("
Configuration
-------------
CMAKE_BUILD_TYPE        = ${CMAKE_BUILD_TYPE}
CXXFLAGS                = $ENV{CXXFLAGS}
CMAKE_CXX_FLAGS         = ${CMAKE_CXX_FLAGS}
LDFLAGS                 = $ENV{LDFLAGS}
CMAKE_EXE_LINKER_FLAGS  = ${CMAKE_EXE_LINKER_FLAGS}
CMAKE_INSTALL_PREFIX    = ${CMAKE_INSTALL_PREFIX}
")

message("You are linking ${PROJECT_NAME}. I hope it makes your life easier.")


# 3rdparty
# SOURCE FILES AND DIRECTORIES

# simple sources
list(APPEND qtjsonsettings_SOURCES qtjsonsettings.cpp json.cpp jsonparser.cpp)

# simple headers
list(APPEND qtjsonsettings_HEADERS json.h)
list(APPEND qtjsonsettings_Q_HEADERS qtjsonsettings.h)

# if you use Q_OBJECT
qt4_wrap_cpp(qtjsonsettings_MOC_SOURCES ${qtjsonsettings_Q_HEADERS})

# COMPILATION
add_library(${PROJECT_NAME} STATIC
  ${qtjsonsettings_SOURCES}
  ${qtjsonsettings_MOC_SOURCES}
  ${qtjsonsettings_HEADERS}
  ${qtjsonsettings_Q_HEADERS}
  )

# LINKING
# main library
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
