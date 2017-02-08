#include <iostream>

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
	catch ( const runtime_error &e )
	{
		cerr << e.what () << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
