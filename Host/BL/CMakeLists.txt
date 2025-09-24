cmake_minimum_required(VERSION 3.16)
project(HostBL)


set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(SRCs src/main.c src/BL.c include/BL.h)
set(CMAKE_C_FLAGS "-lbluetooth")
set(CMAKE_BUILD_TYPE Debug)
add_executable(main ${SRCs})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(main PRIVATE -Wall 
																		-Wextra
																		-Wconversion
																		-Waddress
																		-Wpedantic
																	)
