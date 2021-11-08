cmake_minimum_required(VERSION 3.20)

set(eol_task_scheduler_VERSION_MAJOR 0)
set(eol_task_scheduler_VERSION_MINOR 1)
set(eol_task_scheduler_VERSION_PATCH 0)

set(eol_task_scheduker_VERSION
	${eol_task_scheduler_VERSION_MAJOR}.${eol_task_scheduler_VERSION_MINOR}.${eol_task_scheduler_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for ycm

project(eol_task_scheduler VERSION ${eol_task_scheduker_VERSION})

add_library(eol_task_scheduler INTERFACE)
add_library(eol::task_scheduler ALIAS eol_task_scheduler)

target_include_directories(eol_task_scheduler INTERFACE include/)
add_subdirectory(external/task)
target_link_libraries(eol_task_scheduler INTERFACE eol::task)

option(eol_task_scheduler_TEST "add test executable" ON)

if(eol_task_scheduler_TEST)
	add_executable(eol_task_scheduler_test test/main.cpp)
	target_link_libraries(eol_task_scheduler_test PUBLIC eol::task_scheduler)
endif()
