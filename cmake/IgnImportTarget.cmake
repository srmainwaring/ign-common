#===============================================================================
# Copyright (C) 2017 Open Source Robotics Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
########################################
# ign_import_target(<package> [TARGET_NAME <target_name>])
#
# This macro will create an imported target based on the variables pertaining
# to <package>, such as <package>_LIBRARIES, <package>_INCLUDE_DIRS, and
# <package>_CFLAGS. Optionally, you can provide TARGET_NAME followed by a
# string, which will then be used as the name of the imported target. If
# TARGET_NAME is not provided, the name of the imported target will default to
# <package>::<package>.
#
macro(ign_import_target package)

  #------------------------------------
  # Define the expected arguments
  set(options) # We are not using options yet
  set(oneValueArgs "TARGET_NAME")
  set(multiValueArgs) # We are not using multiValueArgs yet

  #------------------------------------
  # Parse the arguments
  cmake_parse_arguments(ign_import_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  #------------------------------------
  # Check if a target name has been provided, otherwise use
  # ${package}::{$package} as the target name.
  if(ign_import_target_TARGET_NAME)
    set(target_name ${ign_import_target_TARGET_NAME})
  else()
    set(target_name ${package}::${package})
  endif()


  #------------------------------------
  # Link against this "imported" target by saying
  # target_link_libraries(mytarget package::package), instead of linking
  # against the variable package_LIBRARIES with the old-fashioned
  # target_link_libraries(mytarget ${package_LIBRARIES}
  add_library(${target_name} IMPORTED SHARED)

  if(${package}_LIBRARIES)
    _ign_sort_libraries(${target_name} ${${package}_LIBRARIES})
  endif()

  if(${package}_LIBRARIES)
    set_target_properties(${target_name} PROPERTIES
      INTERFACE_LINK_LIBRARIES "${${package}_LIBRARIES}")
  endif()

  message(STATUS "${package}_INCLUDE_DIRS:${${package}_INCLUDE_DIRS}")
  foreach(${package}_inc ${${package}_INCLUDE_DIRS})
    set_target_properties(${target_name} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${${package}_inc}")
  endforeach()

  message(STATUS "${package}_CFLAGS:${${package}_CFLAGS}")
  if(${package}_CFLAGS)
    set_target_properties(${target_name} PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${${package}_CFLAGS}")
  endif()

  # What about linker flags? Is there no target property for that?

endmacro()

# This is an awkward hack to give the package both an IMPORTED_LOCATION and
# a set of INTERFACE_LIBRARIES in the event that PkgConfig returns multiple
# libraries for this package. It seems that IMPORTED_LOCATION cannot support
# specifying multiple libraries, so if we have multiple libraries, we need to
# pass them into INTERFACE_LINK_LIBRARIES. However, if IMPORTED_LOCATION is
# missing from the target, the dependencies do not get configured correctly by
# the generator expressions, and the build system will try to link to a nonsense
# garbage file.
function(_ign_sort_libraries target_name first_lib)

  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${first_lib}")

  foreach(extra_lib ${ARGN})
    set_target_properties(${target_name} PROPERTIES
      INTERFACE_LINK_LIBRARIES "${extra_lib}")
  endforeach()

endfunction()
