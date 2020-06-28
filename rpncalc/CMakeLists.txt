cmake_minimum_required (VERSION 3.15)

add_executable (rpncalc
  "main.cc"
  "rpncalc.h" "rpncalc.cc"
  "rpneval.h" "rpneval.cc"
  "rpnenv.h" "rpnenv.cc"
  "rpncclass.h"
  "rpncon.h" "rpncon.cc"
)

if (MSVC OR MINGW)
  target_sources (rpncalc PRIVATE "rpnconwin32.cc")
else ()
  target_sources (rpncalc PRIVATE "rpnconposix.cc")
endif ()
