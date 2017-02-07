programname="glfw_vulkan"


compiled=1
if [[ $1 == "format" || $1 == "fmt" ]]; then
	clang-format -i main.cc include/* src/*
elif [[ $1 == "win" ]]; then
	clear && /usr/bin/i686-w64-mingw32-g++ ./*.cc ./src/*.cc -o $programname -I include -lvulkan -lglfw -lstbi
	compiled=$?


else
	clear && g++ ./*.cc ./src/*.cc -o $programname -I include -lvulkan -lglfw -lstbi
	compiled=$?
fi
if [[ $1 != "co" && $compiled -eq 0 ]]; then
	./$programname
fi
