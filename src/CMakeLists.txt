set(NAME fecpp)

include_directories(${PYTHON_INCLUDE_DIR})

set(SRC
  fecpp.cpp
  fecpp.h
  fecpp_python.cpp
)

add_library(${NAME} SHARED ${SRC} )

target_link_libraries(${NAME}
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARY}
)


add_library(${NAME}-static STATIC ${SRC} )

target_link_libraries(${NAME}-static
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES}
)
