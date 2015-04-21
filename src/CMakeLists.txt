set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Boost 1.55 REQUIRED)
find_package(Osmium REQUIRED COMPONENTS io sparsehash)

add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "-O3 -g")

add_executable(${PROJECT_NAME} main.cpp mapview.cpp traffic.cpp create_graph.cpp)
target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES} Qt5::Widgets)
