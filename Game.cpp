#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "Level.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>
/*----------*/
#include "Hero.h"
#include "Hero_bullet.h"
#include<iostream>
/*----------*/

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/StartBackground.jpg";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
SceneStart::SceneStart() {
    background = al_load_bitmap("./assets/image/Mywife.jpg");
    font = al_load_font("./assets/font/courbd.ttf", 24, 0);
    menuIndex = 0;
}

SceneStart::~SceneStart() {
    if (background) al_destroy_bitmap(background);
    if (font) al_destroy_font(font);
}

void SceneStart::update() {
    DataCenter* DC = DataCenter::get_instance();
    if (DC->key_state[ALLEGRO_KEY_DOWN] && !DC->prev_key_state[ALLEGRO_KEY_DOWN]) {
        menuIndex = (menuIndex + 1) % 3;
    }
    if (DC->key_state[ALLEGRO_KEY_UP] && !DC->prev_key_state[ALLEGRO_KEY_UP]) {
        menuIndex = (menuIndex - 1 + 3) % 3;
    }
}

void SceneStart::draw() {
    al_draw_bitmap(background, 0, 0, 0);
    const char* options[] = {"START", "MODE", "SETTING"};
    for (int i = 0; i < 3; ++i) {
        ALLEGRO_COLOR color = (i == menuIndex) ? al_map_rgb(255, 0, 0) : al_map_rgb(255, 255, 255);
        al_draw_text(font, color, 400, 200 + i * 50, ALLEGRO_ALIGN_CENTER, options[i]);
    }
}

int SceneStart::getMenuIndex() const {
    return menuIndex;
}

void Game::manage_background_music(bool play) 
{
    SoundCenter* SC = SoundCenter::get_instance();

    if (play) 
    {
        if (!backgroundMusic) 
        {
            // 如果音樂不存在，創建並播放
            backgroundMusic = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
        } 
        else 
        {
            // 如果音樂已經存在但被停止，重新播放
            al_set_sample_instance_playing(backgroundMusic, true);
        }
    } 
    else 
    {
        if (backgroundMusic) 
        {
            al_set_sample_instance_playing(backgroundMusic, false);
        }
    }
}

void Game::execute() 
{
	DataCenter *DC = DataCenter::get_instance();
	// main game loop
	bool run = true;
	while(run) 
	{
		// process all events here
		al_wait_for_event(event_queue, &event);
		switch(event.type) 
		{
			case ALLEGRO_EVENT_TIMER: 
			{
				run &= game_update();
				game_draw();
				break;
			} 
			case ALLEGRO_EVENT_DISPLAY_CLOSE: 
			{ // stop game
				run = false;
				break;
			} 
			case ALLEGRO_EVENT_KEY_DOWN: 
			{
				DC->key_state[event.keyboard.keycode] = true;
				break;
			} 
			case ALLEGRO_EVENT_KEY_UP: 
			{
				DC->key_state[event.keyboard.keycode] = false;
				break;
			} 
			case ALLEGRO_EVENT_MOUSE_AXES: 
			{
				DC->mouse.x = event.mouse.x;
				DC->mouse.y = event.mouse.y;
				break;
			} 
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: 
			{
				DC->mouse_state[event.mouse.button] = true;
				break;
			} 
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP: 
			{
				DC->mouse_state[event.mouse.button] = false;
				break;
			} 
			default: break;
		}
	}
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game() 
{
	DataCenter *DC = DataCenter::get_instance();
	GAME_ASSERT(al_init(), "failed to initialize allegro.");

	// initialize allegro addons
	bool addon_init = true;
	addon_init &= al_init_primitives_addon();
	addon_init &= al_init_font_addon();
	addon_init &= al_init_ttf_addon();
	addon_init &= al_init_image_addon();
	addon_init &= al_init_acodec_addon();
	GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

	// initialize events
	bool event_init = true;
	event_init &= al_install_keyboard();
	event_init &= al_install_mouse();
	event_init &= al_install_audio();
	GAME_ASSERT(event_init, "failed to initialize allegro events.");

	// initialize game body
	GAME_ASSERT(
		display = al_create_display(DC->window_width, DC->window_height),
		"failed to create display.");
	GAME_ASSERT(
		timer = al_create_timer(1.0 / DC->FPS),
		"failed to create timer.");
	GAME_ASSERT(
		event_queue = al_create_event_queue(),
		"failed to create event queue.");

	debug_log("Game initialized.\n");
	game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void Game::game_init() 
{
	DataCenter *DC = DataCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();

	// set window icon
	game_icon = IC->get(game_icon_img_path);
	al_set_display_icon(display, game_icon);

	// register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// init sound setting
	SC->init();

	// init font setting
	FC->init();

	ui = new UI();
	ui->init();

	DC->level->init();
	DC->hero->init();
	startScene = new SceneStart();
	backgroundMusic = nullptr;
	endBackground = al_load_bitmap("./assets/image/Jerry.jpg");

	// game start
	background = IC->get(background_img_path);
	debug_log("Game state: change to START\n");
	state = STATE::START;
	al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */

bool Game::game_update() 
{
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	static ALLEGRO_SAMPLE_INSTANCE *background = nullptr;

	switch(state) 
	{
		case STATE::START: 
		{
			startScene->update();
			//static bool BGM_played = false;
            if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER]) {
                if (startScene->getMenuIndex() == 0) 
				{ // 選擇 "START"
                    debug_log("<Game> state: change to LEVEL\n");
                    state = STATE::LEVEL;
                    DC->level->load_level(1);
					DC->monsters.clear(); // 清空舊怪物
            		//static bool BGM_played = false;   // 重置背景音樂播放標誌
                }
            }
			break;
		} 
		case STATE::LEVEL: 
		{
			OC->update();
			DC->hero->update();
			//DC->level->update();
			manage_background_music(true);

			if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) 
			{
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				state = STATE::PAUSE;
			}
			if(DC->level->remain_monsters() == 0 && DC->monsters.size() == 0) 
			{
				debug_log("<Game> state: change to END\n");
				state = STATE::END;
			}
			if(DC->player->HP == 0) 
			{
				debug_log("<Game> state: change to END\n");
				state = STATE::END;
			}
			break;
		} 
		case STATE::PAUSE: 
		{
			if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) 
			{
				SC->toggle_playing(background);
				debug_log("<Game> state: change to LEVEL\n");
				state = STATE::LEVEL;
			}
			break;
		} 
		case STATE::END: 
		{
			 DataCenter *DC = DataCenter::get_instance();
			 manage_background_music(false);

    		// 按下Enter 後返回 START
    		if (DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]) 
			{
        		debug_log("<Game> state: change to START\n");
        		state = STATE::START;
				DC->player->reset();
    			DC->level->reset();
				DC->monsters.clear();
				DC->hero->reset();
    		}
           return true;
		}
	}
	// If the game is not paused, we should progress update.
	if(state != STATE::PAUSE && state != STATE::END) 
	{
		SC->update();
		if(state != STATE::START) 
		{
			DC->level->update();
			ui->update();
			DC->player->update();
			printf("It's in STATE:LEVEL\n");
		}
	}
	// game_update is finished. The states of current frame will be previous states of the next frame.
	memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
	memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
	return true;
}

/**
 * @brief Draw the whole game and objects.
 */

void Game::game_draw() 
{
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();

	// Flush the screen first.
	al_clear_to_color(al_map_rgb(100, 100, 100));
	switch(state) 
	{
		case STATE::START: 
		{
			startScene->draw();
			break;
		} 
		case STATE::LEVEL: 
		{
			al_draw_bitmap(background, 0, 0, 0);
			if(DC->game_field_length < DC->window_width)
				al_draw_filled_rectangle(
					DC->game_field_length, 0,
					DC->window_width, DC->window_height,
					al_map_rgb(100, 100, 100));
			if(DC->game_field_length < DC->window_height)
				al_draw_filled_rectangle(
					0, DC->game_field_length,
					DC->window_width, DC->window_height,
					al_map_rgb(100, 100, 100));
			
			DC->level->draw();
			DC->hero->draw();
			ui->draw();
			OC->draw();
			break;
		} 
		case STATE::PAUSE: 
		{
			// game layout cover
			al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				DC->window_width/2., DC->window_height/2.,
				ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
			break;
		} 
		case STATE::END: 
		{
			 al_clear_to_color(al_map_rgb(0, 0, 0));

    		// 繪製結束場景背景圖片
    		if (endBackground) {al_draw_bitmap(endBackground, 0, 0, 0);}

    		// 繪製文字 "Game Over"
    		al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
                 	     DC->window_width / 2.0, DC->window_height / 2.0 - 50,
                 		 ALLEGRO_ALIGN_CENTER, "Game Over");

    		al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(200, 200, 200),
                 		 DC->window_width / 2.0, DC->window_height / 2.0 + 50,
                 	     ALLEGRO_ALIGN_CENTER, "Press Space to play again.");

    		al_flip_display();
    		break;
		}
	}
	al_flip_display();
}

Game::~Game() 
{
	delete startScene;
	if (endBackground) al_destroy_bitmap(endBackground);
	if (backgroundMusic) 
	{
        al_stop_sample_instance(backgroundMusic);
        al_destroy_sample_instance(backgroundMusic);
    }
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
}
/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='

佛祖保佑       永無Bug
*/