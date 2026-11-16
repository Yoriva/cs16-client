#ifndef YORIVA_PLUGIN_H
#define YORIVA_PLUGIN_H

#include <stdint.h>

#define YORIVA_PLUGIN_API_VERSION 1

enum yoriva_hook_e
{
	YORIVA_HOOK_CLIENT_INIT = 0,
	YORIVA_HOOK_CLIENT_SHUTDOWN,
	YORIVA_HOOK_CLIENT_CONNECT,
	YORIVA_HOOK_CLIENT_RESET,
	YORIVA_HOOK_PLAYER_MOVE_INIT,
	YORIVA_HOOK_PLAYER_MOVE,
	YORIVA_HOOK_CREATE_MOVE,
	YORIVA_HOOK_FRAME,
	YORIVA_HOOK_PAINT_HUD,
	YORIVA_HOOK_UPDATE_CLIENT_DATA,
	YORIVA_HOOK_PROCESS_PLAYER_STATE,
	YORIVA_HOOK_ADD_ENTITY,
	YORIVA_HOOK_CREATE_ENTITIES,
	YORIVA_HOOK_IN_COMMANDS,
	YORIVA_HOOK_DEATHMSG,
	YORIVA_HOOK_HUD_ELEMENT_DRAW,
	YORIVA_HOOK_NETMSG,
	YORIVA_HOOK_SCOREBOARD_TOGGLE,
	YORIVA_HOOK_AMMO_INFO,
	YORIVA_HOOK_CENTERPRINT,
	YORIVA_HOOK_HUD_ELEMENT_POS,
	YORIVA_HOOK_THIRDPERSON,
	YORIVA_HOOK_CHAT,
	YORIVA_HOOK_COUNT
};

typedef struct yoriva_player_move_init_args_s
{
	void *pmove;
} yoriva_player_move_init_args_t;

typedef struct yoriva_player_move_args_s
{
	void *pmove;
	int server;
} yoriva_player_move_args_t;

typedef struct yoriva_create_move_args_s
{
	float frametime;
	void *cmd;
	int active;
} yoriva_create_move_args_t;

typedef struct yoriva_frame_args_s
{
	double time;
} yoriva_frame_args_t;

typedef struct yoriva_redraw_args_s
{
	float time;
	int intermission;
} yoriva_redraw_args_t;

typedef struct yoriva_update_client_data_args_s
{
	void *client_data;
	float time;
	int hide_hud;
} yoriva_update_client_data_args_t;

typedef struct yoriva_process_player_state_args_s
{
	void *dst;
	const void *src;
} yoriva_process_player_state_args_t;

typedef struct yoriva_add_entity_args_s
{
	int type;
	void *entity;
	const char *modelname;
	int allow;
} yoriva_add_entity_args_t;

typedef struct yoriva_deathmsg_args_s
{
	int killer;
	int victim;
	int headshot;
	const char *weapon;
	const char *killer_name;
	const char *victim_name;
	int killer_team;
	int victim_team;
	int killer_is_local;
	int victim_is_local;
	int is_suicide;
	int is_teamkill;
	int is_nonplayer_kill;
	float time;
} yoriva_deathmsg_args_t;

typedef struct yoriva_hud_element_args_s
{
	void *element;
	const char *name;
	int allow;
} yoriva_hud_element_args_t;

typedef struct yoriva_netmsg_args_s
{
	const char *name;
	int size;
	const void *buf;
} yoriva_netmsg_args_t;

typedef struct yoriva_scoreboard_args_s
{
	int visible;
} yoriva_scoreboard_args_t;

typedef struct yoriva_ammo_args_s
{
	int clip;
	int reserve;
	const char *weapon_name;
} yoriva_ammo_args_t;

typedef struct yoriva_centerprint_args_s
{
	const char *text;
	int allow;
} yoriva_centerprint_args_t;

typedef struct yoriva_hud_element_pos_args_s
{
	const char *name;
	int x;
	int y;
} yoriva_hud_element_pos_args_t;

typedef struct yoriva_thirdperson_args_s
{
	int allow;
} yoriva_thirdperson_args_t;

typedef struct yoriva_chat_args_s
{
	const char *text;
	int client_index;
	float time;
	int allow;
} yoriva_chat_args_t;

typedef void (*yoriva_hook_fn)(void *args, void *user_data);

typedef struct yoriva_plugin_api_s
{
	uint32_t api_version;
	uint32_t flags;
	uint32_t reserved[2];
	void (*register_hook)(int hook_id, yoriva_hook_fn fn, void *user_data);
	void (*unregister_hook)(int hook_id, yoriva_hook_fn fn, void *user_data);
	void (*con_printf)(const char *fmt, ...);
	void *engine_funcs;
	void (*get_screen_size)(int *width, int *height);
	void *render_api;
} yoriva_plugin_api_t;

typedef struct yoriva_plugin_info_s
{
	const char *name;
	const char *version;
	uint32_t api_version;
	int priority;
} yoriva_plugin_info_t;

#ifdef __cplusplus
extern "C" {
#endif

const yoriva_plugin_info_t *yoriva_plugin_query(void);
int yoriva_plugin_init(const yoriva_plugin_api_t *api);

#ifdef __cplusplus
}
#endif

#endif
