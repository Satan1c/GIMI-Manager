cmake_minimum_required(VERSION 3.25)
project(GIMI_Manager)

set(OS_NAME "x64-linux")
set(VCPKG_PATH "/vcpkg-boot/vcpkg")
if (WIN32)
	set(OS_NAME "x64-windows")
	set(VCPKG_PATH "C:/vcpkg")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
include("${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "./src/*.h" "./src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})

target_link_directories(${PROJECT_NAME} PRIVATE
		${VCPKG_PATH}/installed/${OS_NAME}/lib
		${VCPKG_PATH}/installed/${OS_NAME}/lib/manual-link
)
target_include_directories(${PROJECT_NAME} PRIVATE
		${VCPKG_PATH}/installed/${OS_NAME}/lib
		${VCPKG_PATH}/installed/${OS_NAME}/lib/manual-link
		${VCPKG_PATH}/installed/${OS_NAME}/include
)

find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
		imgui::imgui
		SDL2::SDL2
		SDL2::SDL2main
)
