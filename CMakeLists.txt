cmake_minimum_required(VERSION 2.8.3)
project(dcdc_nuc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###########
## Build ##
###########

include_directories(include)

add_executable(${PROJECT_NAME}_console src/${PROJECT_NAME}.cpp src/${PROJECT_NAME}_console.cpp)
target_link_libraries(${PROJECT_NAME}_console ${catkin_LIBRARIES} usb)

#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME}_console
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)
