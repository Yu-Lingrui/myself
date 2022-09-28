
cmake_minimum_required(VERSION 3.0)
PROJECT(EMPLOYEE_SYSTEM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set( CMAKE_BUILD_TYPE "Release")

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC 
    src/CCtrlSystem.cpp 
    src/CWorker.cpp
    src/CEmployee.cpp
    src/CManager.cpp
    src/CBoss.cpp)

add_executable(EmployeeSystem EmployeeSystem.cpp ${SRC})

