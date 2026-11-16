#ifndef _WIN32
#define _GNU_SOURCE 1
#include <dlfcn.h>
#include <dirent.h>
#endif

#include "loader.h"
#include "hud.h"
#include "netadr.h"
#include "pmtrace.h"
#include "pm_shared.h"
#include "APIProxy.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

#ifndef _WIN32
#include <strings.h>
#else
#include <string.h>
#endif

extern cl_enginefunc_t gEngfuncs;

struct YRPluginRecord
{
	void *handle;
	const yoriva_plugin_info_t *info;
	std::string name;
	std::string version;
	int priority;
	bool paused;
};

static std::vector<YRPluginRecord> s_plugins;
static int s_currentPlugin = -1;

#if defined(__ANDROID__)
	#if defined(__aarch64__)
		#define YORIVA_ABI_TAG "arm64"
	#elif defined(__arm__)
		#define YORIVA_ABI_TAG "arm"
	#elif defined(__x86_64__)
		#define YORIVA_ABI_TAG "x86_64"
	#elif defined(__i386__)
		#define YORIVA_ABI_TAG "x86"
	#else
		#define YORIVA_ABI_TAG ""
	#endif
#elif defined(__linux__) || defined(__APPLE__)
	#if defined(__x86_64__)
		#define YORIVA_ABI_TAG "amd64"
	#elif defined(__i386__)
		#define YORIVA_ABI_TAG "i386"
	#elif defined(__aarch64__)
		#define YORIVA_ABI_TAG "arm64"
	#else
		#define YORIVA_ABI_TAG ""
	#endif
#else
	#define YORIVA_ABI_TAG ""
#endif

#if defined(__APPLE__)
	#define YORIVA_PLUGIN_EXT ".dylib"
#else
	#define YORIVA_PLUGIN_EXT ".so"
#endif

static const char *yoriva_abi_suffix( void )
{
	static std::string s;
	if ( s.empty() && YORIVA_ABI_TAG[0] != '\0' )
		s = std::string( "_yoriva_" ) + YORIVA_ABI_TAG + YORIVA_PLUGIN_EXT;
	return s.c_str();
}

static void YR_ConPrintf( const char *fmt, ... )
{
	char buf[1024];
	va_list args;
	va_start( args, fmt );
	vsnprintf( buf, sizeof( buf ), fmt, args );
	va_end( args );
	buf[sizeof( buf ) - 1] = '\0';
	gEngfuncs.Con_Printf( "%s", buf );
}

static void YRPluginAPI_RegisterHook( int hook_id, yoriva_hook_fn fn, void *user_data )
{
	YR_RegisterHook( hook_id, fn, user_data, s_currentPlugin );
}

static void YRPluginAPI_GetScreenSize( int *width, int *height )
{
	if ( width )
		*width = gHUD.m_scrinfo.iWidth;

	if ( height )
		*height = gHUD.m_scrinfo.iHeight;
}

static void YRPluginAPI_UnregisterHook( int hook_id, yoriva_hook_fn fn, void *user_data )
{
	YR_UnregisterHook( hook_id, fn, user_data, s_currentPlugin );
}

static yoriva_plugin_api_t s_plugin_api =
{
	YORIVA_PLUGIN_API_VERSION,
	0,
	{ 0, 0 },
	YRPluginAPI_RegisterHook,
	YRPluginAPI_UnregisterHook,
	YR_ConPrintf,
	&gEngfuncs,
	YRPluginAPI_GetScreenSize,
	&gRenderAPI,
};

extern "C" {

int YRPluginLoader_IsPluginPaused( int id )
{
	if ( id < 0 || id >= (int)s_plugins.size() )
		return 0;
	return s_plugins[id].paused ? 1 : 0;
}

static int YRNameCompare( const char *a, const char *b )
{
#if defined( _WIN32 )
	return _stricmp( a, b );
#else
	return strcasecmp( a, b );
#endif
}

static int YRFindPlugin( const char *token )
{
	if ( !token || !token[0] )
		return -1;

	char *end = nullptr;
	long idx = strtol( token, &end, 10 );

	if ( end && *end == '\0' )
		return ( idx >= 1 && idx <= (long)s_plugins.size() ) ? (int)idx - 1 : -1;

	for ( size_t i = 0; i < s_plugins.size(); i++ )
	{
		if ( YRNameCompare( s_plugins[i].name.c_str(), token ) == 0 )
			return (int)i;
	}

	return -1;
}

static void YRPrintCommandHelp( void )
{
	YR_ConPrintf( "ymod commands:\n" );
	YR_ConPrintf( "  ymod list                 - list loaded plugins\n" );
	YR_ConPrintf( "  ymod info <name|index>    - show plugin details\n" );
	YR_ConPrintf( "  ymod pause <name|index>   - disable a plugin's hooks\n" );
	YR_ConPrintf( "  ymod unpause <name|index> - re-enable a paused plugin\n" );
	YR_ConPrintf( "  ymod toggle <name|index>  - toggle plugin state\n" );
	YR_ConPrintf( "  ymod reload [name|index]  - re-run plugin init (re-registers hooks)\n" );
	YR_ConPrintf( "  ymod help                 - show this help\n" );
}

static void YRPrintPluginList( void )
{
	YR_ConPrintf( "ymod: %d plugin(s) loaded\n", (int)s_plugins.size() );

	for ( size_t i = 0; i < s_plugins.size(); i++ )
	{
		const YRPluginRecord &record = s_plugins[i];
		YR_ConPrintf( "  [%d] %s %s pri=%d hooks=%d %s\n",
			(int)i + 1, record.name.c_str(), record.version.c_str(),
			record.priority, YR_CountHooksForOwner( (int)i ),
			record.paused ? "(paused)" : "(active)" );
	}
}

static void YRPrintPluginInfo( int index )
{
	const YRPluginRecord &record = s_plugins[index];
	YR_ConPrintf( "ymod: plugin [%d]\n", index + 1 );
	YR_ConPrintf( "  name:     %s\n", record.name.c_str() );
	YR_ConPrintf( "  version:  %s\n", record.version.c_str() );
	YR_ConPrintf( "  priority: %d\n", record.priority );
	YR_ConPrintf( "  api:      v%u\n", (unsigned)record.info->api_version );
	YR_ConPrintf( "  hooks:    %d\n", YR_CountHooksForOwner( index ) );
	YR_ConPrintf( "  state:    %s\n", record.paused ? "paused" : "active" );
}

static void YRSetPluginPaused( int index, bool paused )
{
	s_plugins[index].paused = paused;
	YR_ConPrintf( "ymod: %s %s\n", s_plugins[index].name.c_str(),
		paused ? "paused" : "unpaused" );
}

static void YRReloadPlugin( int index )
{
#if defined( _WIN32 )
	YR_ConPrintf( "ymod: reload is not supported on this platform\n" );
	return;
#else
	YRPluginRecord &record = s_plugins[index];

	int (*init)( const yoriva_plugin_api_t * ) =
		(int (*)( const yoriva_plugin_api_t * ))dlsym( record.handle, "yoriva_plugin_init" );

	if ( !init )
	{
		YR_ConPrintf( "ymod: %s missing yoriva_plugin_init\n", record.name.c_str() );
		return;
	}

	s_currentPlugin = index;
	int result = init( &s_plugin_api );
	s_currentPlugin = -1;

	if ( result )
		YR_ConPrintf( "ymod: reloaded %s\n", record.name.c_str() );
	else
		YR_ConPrintf( "ymod: %s reload failed\n", record.name.c_str() );
#endif
}

void YRPluginLoader_Command( void )
{
	if ( gEngfuncs.Cmd_Argc() < 2 )
	{
		YRPrintCommandHelp();
		return;
	}

	const char *sub = gEngfuncs.Cmd_Argv( 1 );

	if ( YRNameCompare( sub, "help" ) == 0 )
	{
		YRPrintCommandHelp();
		return;
	}

	if ( YRNameCompare( sub, "list" ) == 0 )
	{
		YRPrintPluginList();
		return;
	}

	const char *arg = gEngfuncs.Cmd_Argc() > 2 ? gEngfuncs.Cmd_Argv( 2 ) : nullptr;

	if ( YRNameCompare( sub, "reload" ) == 0 )
	{
		if ( !arg )
		{
			for ( size_t i = 0; i < s_plugins.size(); i++ )
				YRReloadPlugin( (int)i );
			return;
		}

		int index = YRFindPlugin( arg );
		if ( index < 0 )
		{
			YR_ConPrintf( "ymod: unknown plugin '%s'\n", arg );
			return;
		}
		YRReloadPlugin( index );
		return;
	}

	int index = YRFindPlugin( arg );

	if ( index < 0 )
	{
		YR_ConPrintf( "ymod: usage: ymod %s <name|index>\n", sub );
		return;
	}

	if ( YRNameCompare( sub, "info" ) == 0 )
	{
		YRPrintPluginInfo( index );
		return;
	}

	if ( YRNameCompare( sub, "pause" ) == 0 )
	{
		YRSetPluginPaused( index, true );
		return;
	}

	if ( YRNameCompare( sub, "unpause" ) == 0 )
	{
		YRSetPluginPaused( index, false );
		return;
	}

	if ( YRNameCompare( sub, "toggle" ) == 0 )
	{
		YRSetPluginPaused( index, !s_plugins[index].paused );
		return;
	}

	YR_ConPrintf( "ymod: unknown command '%s'\n", sub );
	YRPrintCommandHelp();
}

#ifndef _WIN32

static std::string YRModuleDir( void )
{
	Dl_info info = {};
	if ( !dladdr( (void *)&YRModuleDir, &info ) || !info.dli_fname )
		return std::string();

	std::string path( info.dli_fname );
	size_t slash = path.find_last_of( '/' );

	if ( slash == std::string::npos )
		return std::string( "." );

	return path.substr( 0, slash );
}

static bool YRMatchesPluginSuffix( const std::string &name, const std::string &suffix )
{
	if ( name.compare( 0, 3, "lib" ) != 0 )
		return false;

	if ( name.length() <= 3 + suffix.length() )
		return false;

	return name.compare( name.length() - suffix.length(), suffix.length(), suffix ) == 0;
}

void YRPluginLoader_Init( void )
{
	static bool s_initialized = false;

	if ( s_initialized )
		return;

	s_initialized = true;

	gEngfuncs.pfnAddCommand( "ymod", YRPluginLoader_Command );

	const char *suffix = yoriva_abi_suffix();

	if ( !suffix || suffix[0] == '\0' )
	{
		YR_ConPrintf( "Yoriva plugin loader: unsupported platform, no plugins loaded\n" );
		return;
	}

	std::string dir = YRModuleDir();

	if ( dir.empty() )
	{
		YR_ConPrintf( "Yoriva plugin loader: could not determine module directory\n" );
		return;
	}

	DIR *d = opendir( dir.c_str() );

	if ( !d )
	{
		YR_ConPrintf( "Yoriva plugin loader: could not open %s\n", dir.c_str() );
		return;
	}

	std::vector<std::string> candidates;

	while ( const dirent *entry = readdir( d ) )
	{
		std::string name( entry->d_name );

		if ( YRMatchesPluginSuffix( name, suffix ) )
			candidates.push_back( name );
	}

	closedir( d );

	if ( candidates.empty() )
		return;

	std::sort( candidates.begin(), candidates.end() );

	struct yr_loaded_plugin_s
	{
		void *handle;
		const yoriva_plugin_info_t *info;
		std::string path;
	};

	std::vector<yr_loaded_plugin_s> loaded;

	for ( const std::string &name : candidates )
	{
		std::string path = dir + "/" + name;
		void *handle = dlopen( path.c_str(), RTLD_NOW | RTLD_NODELETE );

		if ( !handle )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s failed to load: %s\n", name.c_str(), dlerror() );
			continue;
		}

		const yoriva_plugin_info_t *(*query)( void ) =
			(const yoriva_plugin_info_t *(*)( void ))dlsym( handle, "yoriva_plugin_query" );

		if ( !query )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s missing yoriva_plugin_query\n", name.c_str() );
			continue;
		}

		const yoriva_plugin_info_t *info = query();

		if ( !info )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s returned null plugin info\n", name.c_str() );
			continue;
		}

		if ( info->api_version != YORIVA_PLUGIN_API_VERSION )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s requires API v%u, loader provides v%u\n",
				name.c_str(), info->api_version, ( uint32_t )YORIVA_PLUGIN_API_VERSION );
			continue;
		}

		loaded.push_back( { handle, info, path } );
	}

	std::sort( loaded.begin(), loaded.end(),
		[]( const yr_loaded_plugin_s &a, const yr_loaded_plugin_s &b ) {
			return a.info->priority < b.info->priority;
		} );

	s_plugins.clear();

	for ( const auto &plugin : loaded )
	{
		s_plugins.push_back( { plugin.handle, plugin.info, plugin.info->name,
			plugin.info->version, (int)plugin.info->priority, false } );
	}

	for ( size_t i = 0; i < s_plugins.size(); i++ )
	{
		YRPluginRecord &plugin = s_plugins[i];

		int (*init)( const yoriva_plugin_api_t * ) =
			(int (*)( const yoriva_plugin_api_t * ))dlsym( plugin.handle, "yoriva_plugin_init" );

		if ( !init )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s missing yoriva_plugin_init\n", plugin.name.c_str() );
			plugin.paused = true;
			continue;
		}

		s_currentPlugin = (int)i;

		if ( !init( &s_plugin_api ) )
		{
			YR_ConPrintf( "Yoriva plugin loader: %s failed to initialize\n", plugin.name.c_str() );
			s_currentPlugin = -1;
			plugin.paused = true;
			continue;
		}

		s_currentPlugin = -1;

		YR_ConPrintf( "Yoriva plugin loader: loaded %s %s\n", plugin.name.c_str(), plugin.version.c_str() );
	}
}

#else

void YRPluginLoader_Init( void )
{
	gEngfuncs.Con_Printf( "Yoriva plugin loader: not implemented on this platform\n" );
	gEngfuncs.pfnAddCommand( "ymod", YRPluginLoader_Command );
}

#endif

}
