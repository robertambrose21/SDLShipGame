#pragma once

#include <iostream>

#if !defined(NDEBUG)
#define game_assert(condition)                                              \
do {                                                                        \
    if(!(condition)) {                                                      \
        game_assert_function(#condition, __FUNCTION__, __FILE__, __LINE__); \
        exit(1);                                                            \
    }                                                                       \
} while(0)                                                                   
#else
#define game_assert( ignore ) ((void)0)
#endif

static void game_assert_function(const char * condition, const char * function, const char * file, int line) {
    std::cerr << "assert failed (" << condition << "): function " << 
        function << " file " << file << " line " << line << std::endl;                          
    #if defined( __GNUC__ )                 
    __builtin_trap();                       
    #elif defined( _MSC_VER )               
    __debugbreak();                         
    #endif                                  
}