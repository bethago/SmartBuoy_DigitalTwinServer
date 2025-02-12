cmake_minimum_required(VERSION 3.30)
project(SmartBuoy_DigitalTwinServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

add_executable(main
        main.cpp
        oneM2M.cpp
        TwinDevice.cpp
        UnrealEngine.cpp
)

add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(cpprestsdk CONFIG REQUIRED)
target_link_libraries(main PRIVATE cpprestsdk::cpprest)

find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(main PRIVATE Boost::system)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)