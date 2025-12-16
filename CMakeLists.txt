cmake_minimum_required(VERSION 3.10)
project(gazebo_tools)

find_package(gazebo REQUIRED)
find_package(PkgConfig REQUIRED)
#pkg_check_modules(TBB REQUIRED tbb)
# find_package(TBB REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
# msgpack is header-only, no find_package needed

pkg_check_modules(ZMQ REQUIRED libzmq)

include_directories(${GAZEBO_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${ZMQ_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

# camera2zmq - Gazebo camera to ZMQ with msgpack
add_executable(camera2zmq camera2zmq.cpp)
target_link_libraries(camera2zmq ${GAZEBO_LIBRARIES} ${OpenCV_LIBRARIES} ${ZMQ_LIBRARIES} )


# add_subdirectory(demos)