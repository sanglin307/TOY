# Install script for directory: E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/fastgltf")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fastgltf" TYPE FILE FILES
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/base64.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/core.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/dxmath_element_traits.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/glm_element_traits.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/tools.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/types.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/util.hpp"
    "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/include/fastgltf/math.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/Debug/fastgltf.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/Release/fastgltf.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/MinSizeRel/fastgltf.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/RelWithDebInfo/fastgltf.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake"
         "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "E:/git/TOY/Engine/Source/Thirdparty/fastgltf-0.7.2/vs/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
