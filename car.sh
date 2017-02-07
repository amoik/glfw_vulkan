compiled=1
if [[ $1 == "format" || $1 == "fmt" ]]; then
	clang-format -i main.cc include/* src/*
else
	clear && g++ ./*.cc ./src/*.cc -o program -I include -I /usr/include/drm -I . -lvulkan -lglfw -lstbi
	compiled=$?
fi
if [[ $1 != "co" && $compiled -eq 0 ]]; then
	./program
fi
