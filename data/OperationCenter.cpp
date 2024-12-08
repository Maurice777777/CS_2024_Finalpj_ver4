#include "OperationCenter.h"
#include "DataCenter.h"
#include "../monsters/Monster.h"
#include "../towers/Tower.h"
#include "../towers/Bullet.h"
#include "../Player.h"
#include "../Hero.h"
#include "../Hero_bullet.h"
#include "../Enemy/Enemy_combat.h"
#include "../Hero.h"
#include <cmath>
#define PI 3.14159265358979323846

void OperationCenter::_spawn_enemy_around_hero() 
{
    DataCenter* DC = DataCenter::get_instance();
    Hero* hero = DC->hero; // 獲取角色實例
    double hero_x = hero->shape->center_x();
    double hero_y = hero->shape->center_y();

    double radius = 100.0; // 設定敵人與角色的距離
    int directions = 8;    // 八個方位

    for (int i = 0; i < directions; ++i) {
        double angle = i * (PI / 4); // 每 45 度一個方位
        double spawn_x = hero_x + radius * cos(angle);
        double spawn_y = hero_y + radius * sin(angle);

        // 創建敵人並添加到 DataCenter
        EnemyCombat* enemy = new EnemyCombat(spawn_x, spawn_y, EnemyCombatState::APPEAR, i + 1);
        DC->enemycombat.push_back(enemy);
    }
}


void OperationCenter::update() 
{
	// Update monsters.
	_update_monster();
	// Update towers.
	_update_tower();
	// Update tower bullets.
	_update_towerBullet();
	// If any bullet overlaps with any monster, we delete the bullet, reduce the HP of the monster, and delete the monster if necessary.
	_update_monster_towerBullet();
	// If any monster reaches the end, hurt the player and delete the monster.
	_update_monster_player();

	/*---------*/
	_update_hero_bullet();
	_update_enemy_combat();

	static bool enemies_spawned = false;
    if (!enemies_spawned) 
	{
        _spawn_enemy_around_hero();
        enemies_spawned = true; // 確保只生成一次
    }
	/*---------*/
}

void OperationCenter::_update_monster() 
{
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->update();
}

void OperationCenter::_update_tower() 
{
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->update();
}


void OperationCenter::_update_towerBullet() 
{
	std::vector<Bullet*> &towerBullets = DataCenter::get_instance()->towerBullets;
	for(Bullet *towerBullet : towerBullets)
		towerBullet->update();
	// Detect if a bullet flies too far (exceeds its fly distance limit), which means the bullet lifecycle has ended.
	for(size_t i = 0; i < towerBullets.size(); ++i) 
	{
		if(towerBullets[i]->get_fly_dist() <= 0) 
		{
			towerBullets.erase(towerBullets.begin()+i);
			--i;
		}
	}
}

/*--------------revise--------------*/
void OperationCenter::_update_hero_bullet() 
{
    std::vector<Hero_bullet*> &heroBullets = DataCenter::get_instance()->herobullets;
    
    // Update all hero bullets
    for (Hero_bullet *heroBullet : heroBullets) 
	{
        heroBullet->update();
    }

	// Check and remove bullets that are out of bounds
    for (size_t i = 0; i < heroBullets.size(); ++i) 
	{
        if (heroBullets[i]->is_out_of_bound()) 
		{
            delete heroBullets[i]; // Clean up memory
            heroBullets.erase(heroBullets.begin() + i); // Remove from vector
            --i; // Adjust the index after erasure
        }
    }
}

void OperationCenter::_update_enemy_combat()
{
	DataCenter*DC = DataCenter::get_instance();
	std::vector<EnemyCombat*>& ec = DC->enemycombat;

	for(size_t i=0;i<ec.size();++i)
	{
		ec[i]->update();

		if(ec[i]->be_killed())
		{
			delete ec[i];
			ec.erase(ec.begin()+i);
			--i;
		}

	}
}

/*--------------revise--------------*/

void OperationCenter::_update_monster_towerBullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster*> &monsters = DC->monsters;
	std::vector<Bullet*> &towerBullets = DC->towerBullets;
	for(size_t i = 0; i < monsters.size(); ++i) {
		for(size_t j = 0; j < towerBullets.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if(monsters[i]->shape->overlap(*(towerBullets[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				monsters[i]->HP -= towerBullets[j]->get_dmg();
				towerBullets.erase(towerBullets.begin()+j);
				--j;
			}
		}
	}
}

void OperationCenter::_update_monster_player() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster*> &monsters = DC->monsters;
	Player *&player = DC->player;
	for(size_t i = 0; i < monsters.size(); ++i) {
		// Check if the monster is killed.
		if(monsters[i]->HP <= 0) {
			// Monster gets killed. Player receives money.
			player->coin += monsters[i]->get_money();
			monsters.erase(monsters.begin()+i);
			--i;
			// Since the current monsster is killed, we can directly proceed to next monster.
			break;
		}
		// Check if the monster reaches the end.
		if(monsters[i]->get_path().empty()) {
			monsters.erase(monsters.begin()+i);
			player->HP--;
			--i;
		}
	}
}

void OperationCenter::draw() 
{
	_draw_monster();
	_draw_tower();
	_draw_towerBullet();
	_draw_hero_bullet();
	_draw_enemy_combat();
}

void OperationCenter::_draw_monster() {
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->draw();
}

void OperationCenter::_draw_tower() {
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->draw();
}

void OperationCenter::_draw_towerBullet() {
	std::vector<Bullet*> &towerBullets = DataCenter::get_instance()->towerBullets;
	for(Bullet *towerBullet : towerBullets)
		towerBullet->draw();
}

void OperationCenter::_draw_hero_bullet() {
	std::vector<Hero_bullet*> &herobullets = DataCenter::get_instance()->herobullets;
	for(Hero_bullet *herobullet : herobullets)
		herobullet->draw();
}

/*-------Revise--------*/
void OperationCenter::_draw_enemy_combat() 
{
    DataCenter* DC = DataCenter::get_instance();
    std::vector<EnemyCombat*>& enemies = DC->enemycombat;

    for (EnemyCombat* enemy : enemies) {
        enemy->draw();
    }
}
/*-------Revise------*/
