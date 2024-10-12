cmake_minimum_required(VERSION 3.16)
project(cram)

set(COMPILE_NAME ${PROJECT_NAME})
  
set(FRONTEND
  frontend
)
message(STATUS "Collected frontend: ${FRONTEND}")

foreach(FRONT ${FRONTEND})
  add_subdirectory(${FRONT})
endforeach()

foreach(FRONT ${FRONTEND})
  target_include_directories(${FRONT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/${FRONT})
  target_include_directories(${FRONT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  foreach(DIR ${LEXPARSE})
    target_include_directories(${FRONT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/${FRONT}/${DIR})
    target_include_directories(${FRONT} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${FRONT}/${DIR})
  target_link_libraries(${FRONT} )

  endforeach()
endforeach()

set(BACKEND
  backend
)

add_executable(${COMPILE_NAME} 
  driver/driver.cpp
)

set(TARGETS
  ${COMPILE_NAME}
)

# common for all targets
foreach(TNAME ${TARGETS})
  target_compile_features(${TNAME} PRIVATE cxx_std_20)
  foreach(FRONT ${FRONTEND})
    target_include_directories(${TNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/${FRONT})
  endforeach()
  foreach(BACK ${BACKEND})
    target_include_directories(${TNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/${BACK})
  endforeach()
  target_link_libraries(${TNAME} ${FRONTEND})
endforeach()
