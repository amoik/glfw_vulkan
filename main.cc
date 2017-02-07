#include <iostream>
#include <unistd.h>
#include <vulkan/vulkan.h>

#include "Window.class.hh"

using namespace std;


int main ( int ac, char **av )
{
	Window win;
	win.setXSize ( 800 );
	win.setYSize ( 600 );

	try
	{
		win.run ();
	}
	catch ( const std::runtime_error &e )
	{
		std::cerr << e.what () << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
