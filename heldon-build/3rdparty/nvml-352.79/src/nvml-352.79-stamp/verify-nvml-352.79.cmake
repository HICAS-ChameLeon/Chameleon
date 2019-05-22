# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if("/home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz" STREQUAL "")
  message(FATAL_ERROR "LOCAL can't be empty")
endif()

if(NOT EXISTS "/home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz")
  message(FATAL_ERROR "File not found: /home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz")
endif()

if("" STREQUAL "")
  message(WARNING "File will not be verified since no URL_HASH specified")
  return()
endif()

if("" STREQUAL "")
  message(FATAL_ERROR "EXPECT_VALUE can't be empty")
endif()

message(STATUS "verifying file...
     file='/home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz'")

file("" "/home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz" actual_value)

if(NOT "${actual_value}" STREQUAL "")
  message(FATAL_ERROR "error:  hash of
  /home/lemaker/open-source/Chameleon/3rdparty/nvml-352.79.tar.gz
does not match expected value
  expected: ''
    actual: '${actual_value}'
")
endif()

message(STATUS "verifying file... done")
