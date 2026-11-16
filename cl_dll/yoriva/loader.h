#ifndef YORIVA_LOADER_H
#define YORIVA_LOADER_H

#include <yoriva/yoriva_plugin.h>

#ifdef __cplusplus
extern "C" {
#endif

void YRPluginLoader_Init( void );
void YRPluginLoader_Dispatch( int hook_id, void *args );
void YRPluginLoader_Command( void );
int YRPluginLoader_IsPluginPaused( int id );
int YR_CountHooksForOwner( int owner );

void YR_RegisterHook( int hook_id, yoriva_hook_fn fn, void *user_data, int owner );
void YR_UnregisterHook( int hook_id, yoriva_hook_fn fn, void *user_data, int owner );

#ifdef __cplusplus
}
#endif

#endif
