// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

#ifdef __cplusplus
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#endif
