#!/bin/bash
# Compile And Run script

# CLEAR
clear

# CONFIG
bin="glfw_vulkan"
error_log="compile_error.txt"
out_log="compile_output.txt"
CC="g++"
CCflags="-std=c++11" # -fuse-ld=gold"
src="main.cc src/*.cc"
inc="-I ./include"




if [[ $1 == "format" || $1 == "fmt" ]]; then
	clang-format -i $src
else


	# CONFIG
	is_configured=1
	if [ `uname` == "Linux" ] ; then
		ldflags=""
		libs="-lvulkan -lglfw -lstbi"
		libDir=""
		inc_spec=""


	elif [ `uname` == "Darwin" ] ; then # TODO
		ldflags=""
		libs=""
		libDir=""
		inc_spec=""


	elif [ `uname` == "MINGW64_NT-6.1" ] ; then # TODO
		ldflags=""
		libs="-lglfw3.dll" # -lgdi32 -lpthreads -lglfw3  -l /usr/lib/libglfw3.a -lvulkan-1
		libDir="-L /c/usr/lib/"
		inc_spec="-I /c/usr/include/ -I /c/usr/include/stb"


	else
		is_configured=0
	fi


	# Build the full includes
	inc_full="$inc $inc_spec"

	# COLORS
	COLOR_ERROR='\033[0;31m'
	COLOR_NONE='\033[0m'
	COLOR_SUCCESS='\033[0;32m'
	COLOR_WARNING='\033[1;33m'
	COLOR_INFO='\033[1;33m'


	# CHECK FOR ERRORS
	if [ is_configured == 0 ] ; then
		printf "${COLOR_ERROR}Unknown Platform${COLOR_NONE}\n"
		exit
	fi


	# COMPILING
	FULLCOMMAND="$CC $src $CCflags $libDir -o $bin $inc_full $ldflags $libs"
	$FULLCOMMAND 1> $out_log 2> $error_log
	compiled=$?
	terminal_height=`tput lines`
	terminal_width=`tput cols`


	printf "Compiling...\t\t${COLOR_NONE}"
	if [ $compiled == 0 ] ; then
		printf "${COLOR_SUCCESS}Done\n\n${COLOR_NONE}"
		if [ $1 != "co" ]; then
			./$bin
		fi
	else
		printf "${COLOR_ERROR}Failed\n${COLOR_NONE}"
		printf "${COLOR_INFO}$FULLCOMMAND\n\n${COLOR_NONE}"
		out_height=`wc -l $error_log | sed -e "s/\ $error_log//g"`

		if [ $out_height -lt $terminal_height ] ; then
			cat $error_log
		else
			less $error_log
		fi
	fi
fi
