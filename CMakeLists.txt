cmake_minimum_required(VERSION 3.25)
project(GIMI_Manager)

set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(OS_NAME "x64-linux")
set(VCPKG_PATH "/vcpkg-boot/vcpkg")
if (WIN32)
	set(OS_NAME "x64-windows")
	set(VCPKG_PATH "C:/vcpkg")
endif ()

set(CMAKE_CXX_STANDARD 17)

set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
)
foreach (CompilerFlag ${CompilerFlags})
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach ()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
include("${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "./src/*.h" "./src/*.hpp" "./src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})

target_compile_definitions(${PROJECT_NAME} PRIVATE
		WINDOWS_NATIVE
		#SDL2_VULKAN
)
set(WINDOWS_NATIVE true)

target_link_directories(${PROJECT_NAME} PRIVATE
		${VCPKG_PATH}/installed/${OS_NAME}-static/lib
		${VCPKG_PATH}/installed/${OS_NAME}-static/lib/manual-link
)
target_include_directories(${PROJECT_NAME} PRIVATE
		${VCPKG_PATH}/installed/${OS_NAME}-static/lib
		${VCPKG_PATH}/installed/${OS_NAME}-static/lib/manual-link
		${VCPKG_PATH}/installed/${OS_NAME}-static/include
)

find_package(LibArchive REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

if (WINDOWS_NATIVE)
	find_package(DirectXTK CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE
			imgui::imgui
			d3d11.lib dxgi.lib dxguid.lib uuid.lib
			kernel32.lib user32.lib
			comdlg32.lib advapi32.lib shell32.lib
			ole32.lib oleaut32.lib
			Microsoft::DirectXTK
			unofficial::sqlite3::sqlite3
			LibArchive::LibArchive
	)
else ()
	find_package(SDL2 CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE
			imgui::imgui
			SDL2::SDL2main
			SDL2::SDL2-static
			unofficial::sqlite3::sqlite3
			LibArchive::LibArchive
	)
endif ()
