#################################################################################################
## File created by QxEntityEditor 1.2.5 (2020/07/13 22:52) : please, do NOT modify this file ! ##
#################################################################################################

cmake_minimum_required(VERSION 3.1)

project(Licenta LANGUAGES CXX)

if(NOT TARGET QxOrm)
   add_subdirectory("C:/Qt/QxOrm/" ./QxOrm)
endif() # NOT TARGET QxOrm

include("C:/Qt/QxOrm/QxOrm.cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX "d")

set(LICENTA_HEADERS
    ./include/Licenta_precompiled_header.gen.h
    ./include/Licenta_export.gen.h
    ./include/Camera.gen.h
    ./include/Violation.gen.h
    ./custom/include/Camera.h
    ./custom/include/Violation.h
   )

if(_QX_UNITY_BUILD)

   set(LICENTA_SRCS ./src/Licenta_all.gen.cpp)

else() # _QX_UNITY_BUILD

   set(LICENTA_SRCS
       ./src/Camera.gen.cpp
       ./src/Violation.gen.cpp
       ./custom/src/Camera.cpp
       ./custom/src/Violation.cpp
       ./src/Licenta_main.gen.cpp
      )

endif() # _QX_UNITY_BUILD

add_library(Licenta SHARED ${LICENTA_SRCS} ${LICENTA_HEADERS})

target_compile_definitions(Licenta PRIVATE -D_BUILDING_LICENTA)

target_link_libraries(Licenta ${QX_LIBRARIES} QxOrm)

set_target_properties(Licenta PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin"
                     )

qx_auto_source_group("${LICENTA_SRCS};${LICENTA_HEADERS}")
