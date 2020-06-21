cmake_minimum_required (VERSION 3.15)

project ("rpncalc" LANGUAGES CXX)

set (CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)

if (MSVC)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:/W3>)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:/w34389>)
else ()
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:-Wall>)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:-Wconversion>)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:-Wsign-compare>)
  add_compile_options ($<$<COMPILE_LANGUAGE:CXX>:-fdiagnostics-color>)
endif ()

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory ("rpncalc")
