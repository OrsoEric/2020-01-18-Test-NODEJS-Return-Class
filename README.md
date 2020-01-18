# 2020-01-18-Test-NODEJS-Return-Class
 return a custom type/object from C++ to NODE.JS
**Setup:**<br>
I have a NODE.JS application that must perform some computation at low latency<br>
I decided to use N-API and node-gyp to include a native C++ module to the NODE.JS application<br>

**Current Status:**<br>
I got to the point where the toolchain works, I can compile the C++ source code into binaries, include the binary module in the NODE.JS application, the NODE.JS application executes<br>
I can call C++ methods with standard NODE.JS types from NODE.JS and the methods can return standard NODE.JS types back to NODE.JS when they are done with execution<br>

**Problem:**<br>
I can't figure out how to return a custom type/object from C++ to NODE.JS<br>
Currently I want to return basically a structure with multiple types, in order to get the result of complex parsing back to NODE.JS in a single NODE.JS call<br>

**Minimal code output:**<br>
I made a minimal implementation to demonstrate whet I want to do.
If you comment ```#define ENABLE_RETURN_CLASS``` the code only uses standard NODE.JS types and everything works. Below an image of the output, showing the toolchain and execution works as intended:<br>
[![float works][1]][1]

If you leave ```#define ENABLE_RETURN_CLASS``` the code fails to compile. It doesn't understand how to convert from a C++ object to a NODE.JS object as far as I understand. This is the error:
[![enter image description here][2]][2]

**Minimal code:**<br>
Initialize NODE.JS application
```
npm init
npm install node-gyp --save-dev
npm install node-addon-api
```
Compile C++ binaries into a NODE.JS module<br>
```
npm run build
```
Launch the NODE.JS application<br
```
node main.js
```
The code can be found in this reposiory:<br>
[https://github.com/OrsoEric/2020-01-18-Test-NODEJS-Return-Class][3]<br>
I plan to update it once a solution is found.

Code for the class I want to return to the NODE.JS application:<br>

***my_class.h***<br>
```
namespace User
{
	//Class I want to return to NODE.JS
	class My_class
	{
		public:
			//Constructor
			My_class( void );
			//Public references
			float &my_float( void );
			int &my_int( void );
		private:
			//Private class vars
			float g_my_float;
			int g_my_int;
	};
}	//End namestace: User
```
***my_class.cpp***<br>
```
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

```
Code for the bindings between C++ and NODE.JS. ```#define ENABLE_RETURN_CLASS``` enables the code that returns the class. The instance in this example is a global variable.<br>

***node_bindings.cpp***<br>
```
//NODE bindings
#include <napi.h>
//C++ Class I want to return to NODE.JS
#include "my_class.h"

//Comment to disable the code that return the class instance
//#define ENABLE_RETURN_CLASS

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
    return Napi::Object::New(env, (User::My_class)tmp);
} //End Function: get_my_float | Napi::CallbackInfo&
#endif // ENABLE_RETURN_CLASS

NODE_API_MODULE( My_cpp_module, init )
```
The NODE.JS application ***main.js*** include and executes the C++ module:<br>
```
//Include native C++ module
const my_custom_cpp_module = require('./build/Release/MyCustomCppModule.node');
console.log('My custom c++ module',my_custom_cpp_module);

//TEST:
tmp = my_custom_cpp_module.get_my_float();
console.log( tmp );

module.exports = my_custom_cpp_module;

```

The bindings are described in the file ***binding.gyp***:<br>
```
{
    "targets": [{
        "target_name": "MyCustomCppModule",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "node_bindings.cpp",
			"my_class.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'libraries': [],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}
```
While ***package.json*** is what NODE.JS needs to resolve dependencies, compile and run<br>
```
{
  "name": "2020-01-18-test-return-object",
  "version": "1.0.0",
  "description": "",
  "main": "main.js",
  "scripts": {
    "build": "node-gyp rebuild",
	"clean": "node-gyp clean"
  },
  "author": "",
  "license": "ISC",
  "gypfile": true,
  "devDependencies": {
    "node-gyp": "^6.1.0"
  },
  "dependencies": {
    "node-addon-api": "^2.0.0"
  }
}
```

----------
<h1>SOLUTION</h1>
I can't fit a custom class inside Napi::Object, but I can create an empty Napi::Object and create fields one by one.
https://github.com/OrsoEric/2020-01-18-Test-NODEJS-Return-Class

Implement the correct construction of a Napi::Object inside ***node_bindings.cpp***<br>
```
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
	//Get a copy of the instance of the class I want to return
	User::My_class tmp = g_instance;
	//Construct empty return object in the NODE.JS environment
	Napi::Object ret_tmp = Napi::Object::New( env );
	//Manually create and fill the fields of the return object
	ret_tmp.Set("my_float", Napi::Number::New( env, (float)tmp.my_float() ));
	ret_tmp.Set("my_int", Napi::Number::New( env, (int)tmp.my_int() ));
	//Return a NODE.JS Object
    return (Napi::Object)ret_tmp;
} //End Function: get_my_class | Napi::CallbackInfo&
#endif // ENABLE_RETURN_CLASS

NODE_API_MODULE( My_cpp_module, init )
```

Add the test instruction in ***main.js***:<br>
```
//Include native C++ module
const my_custom_cpp_module = require('./build/Release/MyCustomCppModule.node');
console.log('My custom c++ module',my_custom_cpp_module);

//TEST: Standard NODE.JS type
tmp = my_custom_cpp_module.get_my_float();
console.log( tmp );
//Custom NODE.JS type
class_tmp = my_custom_cpp_module.get_my_class();
console.log( class_tmp );

module.exports = my_custom_cpp_module;

```

***Output:***<br>
[![enter image description here][4]][4]


  [1]: https://i.stack.imgur.com/IJK9S.png
  [2]: https://i.stack.imgur.com/ZdrvS.png
  [3]: https://github.com/OrsoEric/2020-01-18-Test-NODEJS-Return-Class
  [4]: https://i.stack.imgur.com/vQjaS.png
