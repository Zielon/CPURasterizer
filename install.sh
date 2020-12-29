#!/bin/sh

green='\033[1;32m'
nc='\033[0m'

root_path=$(dirname "${0}")
root_path=$(realpath "${root_path}")

mkdir Build
cd Build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
git checkout 2020.11
./bootstrap-vcpkg.sh

./vcpkg install \
	glad:x64-linux \
	glfw3:x64-linux \
	glm:x64-linux \
	imgui:x64-linux \
	stb:x64-linux \
	tinyobjloader:x64-linux

echo -e " ${green}*${nc} Make sure to set VCPKG_ROOT enviroment variable prior"
echo -e " ${green}*${nc} to configuring cmake. E.g.:"
echo -e " ${green}*${nc}   VCPKG_ROOT=\"${root_path}/Build/vcpkg.linux\" cmake <...>"
