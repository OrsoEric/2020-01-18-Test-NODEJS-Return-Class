//NODE bindings
#include <napi.h>
//C++ Class I want to return to NODE.JS
#include "my_class.h"

//Comment to disable the code that return the class instance
#define ENABLE_RETURN_CLASS

//Instance of My_class I want to return to NODE.JS
User::My_class g_instance;

//Prototype of function called by NODE.JS that initializes this module
extern Napi::Object init(Napi::Env env, Napi::Object exports);
//Prototype of function that returns a standard type: WORKS
extern Napi::Number get_my_float(const Napi::CallbackInfo& info);

#ifdef ENABLE_RETURN_CLASS
//Prototype of function that returns My_class to NODE.JS: DOES NOT WORK!!!
extern Napi::Object get_my_class(const Napi::CallbackInfo& info);
#endif // ENABLE_RETURN_CLASS

//Initialize instance
Napi::Object init(Napi::Env env, Napi::Object exports)
{
	//Construct the instance of My_class I want to return to NODE.JS
	g_instance = User::My_class();
		//Register methods accessible from the outside in the NODE.JS environment
	//Return a standard type
	exports.Set( "get_my_float", Napi::Function::New(env, get_my_float) );
	#ifdef ENABLE_RETURN_CLASS
	//Return the whole class
	exports.Set( "get_my_class", Napi::Function::New(env, get_my_class) );
	#endif

    return exports;
}	//End function: init | Napi::Env | Napi::Object

//Interface between function and NODE.JS
Napi::Number get_my_float(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
	//Check arguments
    if (info.Length() != 0)
	{
		Napi::TypeError::New(env, "ERR: Expecting no arguments").ThrowAsJavaScriptException();
	}
	//Get the return value
	float tmp = g_instance.my_float();
	//Return a NODE.JS number
    return Napi::Number::New(env, (float)tmp);
} //End Function: get_my_float | Napi::CallbackInfo&

#ifdef ENABLE_RETURN_CLASS
//Interface between function and NODE.JS
Napi::Object get_my_class(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
	//Check arguments
    if (info.Length() != 0)
	{
		Napi::TypeError::New(env, "ERR: Expecting no arguments").ThrowAsJavaScriptException();
	}
	//Get the return value
	User::My_class tmp = g_instance;
	//Return a NODE.JS number
    return Napi:Object::New(env, (User::My_class)tmp);
} //End Function: get_my_float | Napi::CallbackInfo&
#endif // ENABLE_RETURN_CLASS

NODE_API_MODULE( My_cpp_module, init )
