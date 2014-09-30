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

#SET_TARGET_PROPERTIES(${NAME} PROPERTIES LINK_FLAGS "-lboost_python -L/usr/lib/python2.7/config -lpython2.7")

if (APPLE)
    set_property(TARGET ${NAME} PROPERTY PREFIX "lib")
    set_property(TARGET ${NAME} PROPERTY OUTPUT_NAME "fecpp.so")
    set_property(TARGET ${NAME} PROPERTY SUFFIX "")
endif()


#add_library(${NAME}-static STATIC ${SRC} )

#target_link_libraries(${NAME}-static
#  ${Boost_LIBRARIES}
#  ${PYTHON_LIBRARIES}
#)
