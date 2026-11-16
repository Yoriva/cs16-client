#include "loader.h"
#include <vector>
#include <mutex>

extern "C" {

struct yr_hook_entry_s
{
	yoriva_hook_fn fn;
	void *user_data;
	int owner;
};

static std::vector<yr_hook_entry_s> s_hooks[YORIVA_HOOK_COUNT];
static std::mutex s_hook_mutex;

void YR_RegisterHook( int hook_id, yoriva_hook_fn fn, void *user_data, int owner )
{
	if ( !fn || hook_id < 0 || hook_id >= YORIVA_HOOK_COUNT )
		return;

	std::lock_guard<std::mutex> lock( s_hook_mutex );

	for ( const auto &entry : s_hooks[hook_id] )
	{
		if ( entry.fn == fn && entry.user_data == user_data && entry.owner == owner )
			return;
	}

	s_hooks[hook_id].push_back( { fn, user_data, owner } );
}

void YR_UnregisterHook( int hook_id, yoriva_hook_fn fn, void *user_data, int owner )
{
	if ( !fn || hook_id < 0 || hook_id >= YORIVA_HOOK_COUNT )
		return;

	std::lock_guard<std::mutex> lock( s_hook_mutex );
	auto &hooks = s_hooks[hook_id];

	for ( auto it = hooks.begin(); it != hooks.end(); )
	{
		if ( it->fn == fn && it->user_data == user_data && ( owner < 0 || it->owner == owner ) )
			it = hooks.erase( it );
		else
			++it;
	}
}

int YR_CountHooksForOwner( int owner )
{
	std::lock_guard<std::mutex> lock( s_hook_mutex );
	int count = 0;

	for ( int id = 0; id < YORIVA_HOOK_COUNT; id++ )
	{
		for ( const auto &entry : s_hooks[id] )
		{
			if ( entry.owner == owner )
				++count;
		}
	}

	return count;
}

void YRPluginLoader_Dispatch( int hook_id, void *args )
{
	if ( hook_id < 0 || hook_id >= YORIVA_HOOK_COUNT )
		return;

	std::vector<yr_hook_entry_s> snapshot;

	{
		std::lock_guard<std::mutex> lock( s_hook_mutex );
		snapshot = s_hooks[hook_id];
	}

	for ( const auto &entry : snapshot )
	{
		if ( entry.owner >= 0 && YRPluginLoader_IsPluginPaused( entry.owner ) )
			continue;

		entry.fn( args, entry.user_data );
	}
}

}
