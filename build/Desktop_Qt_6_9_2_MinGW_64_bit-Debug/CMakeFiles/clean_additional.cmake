# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SmartDict_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SmartDict_autogen.dir\\ParseCache.txt"
  "SmartDict_autogen"
  )
endif()
