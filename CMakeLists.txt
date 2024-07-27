cmake_minimum_required  (VERSION 2.8.7)

project  (vSomeIPServiceExample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11")

find_package(Threads  REQUIRED)

include_directories("/home/ml/car/vsomeip")

link_directories("/usr/local/lib")

include_directories(${VSOMEIP_INCLUDE_DIRS})

include_directories(${Boost_INCLUDE_DIR})

find_package(vsomeip3 REQUIRED)

add_executable  (service-example ../service-example.cpp)

target_link_libraries(service-example  vsomeip3 pthread ${Boost_LIBRARIES})


add_executable  (client-example ../client-example.cpp)

target_link_libraries(client-example  vsomeip3 pthread ${Boost_LIBRARIES})

add_executable  (client2 ../client2.cpp)

target_link_libraries(client2  vsomeip3 pthread ${Boost_LIBRARIES})
