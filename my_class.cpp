#include <iostream>
//Class header
#include "my_class.h"

namespace User
{
	//Constructor
	My_class::My_class( void )
	{
        this -> g_my_float = (float)1.001;
        this -> g_my_int = (int)42;
	}
	//Public Reference
	float &My_class::my_float( void )
	{
		return this -> g_my_float;
	}
	//Public Reference
	int &My_class::my_int( void )
	{
		return this -> g_my_int;
	}
}	//End namestace: User

