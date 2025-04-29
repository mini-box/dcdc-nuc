cmake_minimum_required(VERSION 3.5)
project(dcdc_nuc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# catkin for ROS if found
find_package(catkin QUIET)

add_executable(
  ${PROJECT_NAME}_console
  src/${PROJECT_NAME}.cpp
  src/${PROJECT_NAME}_console.cpp
)

target_include_directories(
  ${PROJECT_NAME}_console PRIVATE
  include
  ${catkin_INCLUDE_DIRS}
)

target_link_libraries(
  ${PROJECT_NAME}_console
  usb
)

if(catkin_FOUND)
  target_link_libraries(
    ${PROJECT_NAME}_console
    ${catkin_LIBRARIES}
  )
endif()

install(
  TARGETS ${PROJECT_NAME}_console
  RUNTIME DESTINATION bin
)
