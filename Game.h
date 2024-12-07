#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "UI.h"

/**
 * @brief Main class that runs the whole game.
 * @details All game procedures must be processed through this class.
 */
class SceneStart 
{
private:
    ALLEGRO_BITMAP* background;
    ALLEGRO_FONT* font;
    int menuIndex;

public:
    SceneStart();
    ~SceneStart();
    void update();
    void draw();
    int getMenuIndex() const;
};

class Game
{
	public:
		void execute();
	public:
		Game();
		~Game();
		void game_init();
		bool game_update();
		void game_draw();
	private:
		/**
	 	* @brief States of the game process in game_update.
	 	* @see Game::game_update()
	 	*/
		enum class STATE {
			START, // -> LEVEL
			LEVEL, // -> PAUSE, END
			PAUSE, // -> LEVEL
			END
		};
		STATE state;
		ALLEGRO_EVENT event;
		ALLEGRO_BITMAP *game_icon;
		ALLEGRO_BITMAP *background;
		ALLEGRO_BITMAP* endBackground;
	private:
		ALLEGRO_DISPLAY *display;
		ALLEGRO_TIMER *timer;
		ALLEGRO_EVENT_QUEUE *event_queue;
		UI *ui;
	 	SceneStart* startScene; 
		ALLEGRO_SAMPLE_INSTANCE* backgroundMusic;
};

#endif