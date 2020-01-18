//Include native C++ module
const my_custom_cpp_module = require('./build/Release/MyCustomCppModule.node');
console.log('My custom c++ module',my_custom_cpp_module);

//TEST:
tmp = my_custom_cpp_module.get_my_float();
console.log( tmp );

module.exports = my_custom_cpp_module;
