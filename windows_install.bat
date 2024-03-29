IF not EXIST Build (
	mkdir Build
	cd Build
	git clone https://github.com/Microsoft/vcpkg.git vcpkg.windows
	cd vcpkg.windows
	git checkout 2021.05.12
	call bootstrap-vcpkg.bat

	vcpkg.exe install ^
		glad:x64-windows ^
		glfw3:x64-windows ^
		glm:x64-windows ^
		imgui:x64-windows ^
		stb:x64-windows ^
		tinyobjloader:x64-windows

	vcpkg.exe integrate install
	cd ../..
)

cd Rasterizer/Assets

IF not EXIST PBRScenes (
	git clone https://github.com/Zielon/PBRScenes
)

exit /b
