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
