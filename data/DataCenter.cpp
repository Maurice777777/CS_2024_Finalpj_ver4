#include "DataCenter.h"
#include <cstring>
#include "../Level.h"
#include "../Player.h"
#include "../monsters/Monster.h"
#include "../towers/Tower.h"
#include "../towers/Bullet.h"
/*---------*/
#include "../Hero.h"
#include "../Hero_bullet.h"
#include "../Enemy/Enemy_combat.h"
/*---------*/

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 800;
	constexpr int window_height = 600;
	constexpr int game_field_length = 600;
}

DataCenter::DataCenter() 
{
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));
	player = new Player();
	level = new Level();
	/*--------*/
	hero = new Hero();
	//bullet = new Hero_bullet();
	/*--------*/
}

DataCenter::~DataCenter() {
	delete player;
	delete level;
	for(Monster *&m : monsters) {
		delete m;
	}
	for(Tower *&t : towers) {
		delete t;
	}
	for(Bullet *&b : towerBullets) {
		delete b;
	}
	for(Hero_bullet *&hb : herobullets)
	{
		delete hb;
	}
	for(EnemyCombat *&ec : enemycombat)
	{
		delete ec;
	}
}
