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
#        TwinDevice.cpp
#        UnrealEngine.cpp
#        WriteLog.cpp
#        ReadLog.cpp
)

find_package(cpprestsdk CONFIG REQUIRED)
target_link_libraries(main PRIVATE cpprestsdk::cpprest)
