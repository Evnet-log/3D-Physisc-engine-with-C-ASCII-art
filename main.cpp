#include <stdio.h>
#include <windows.h>
#include <cmath>
#include <time.h>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <tuple>

#include "thing.h"
#include "GJK.h"
#include "container.h"
#include "renderer.h"
#include "camera.h"
#include "game.h"

#define SCREEN_X 240
#define SCREEN_Y 75

int main()
{
	srand(time(NULL));
	system("mode con: cols=260 lines=75");
	c();

	const std::chrono::milliseconds frame_duration(1000 / 250);

	auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	// renderer
	RENDERER renderer(SCREEN_X, SCREEN_Y);
	
	// container
	CONTAINER container;
	
	container.set_accel({0.0f, -20.0f, 0.0f});
	container.set_dt(1.0f/60.0f);
	container.set_repeat(20);
	
	// camera
	CAMERA camera;

	camera.set_fov(M_PI/3.0f);
	camera.set_pos({0.0f, 9.0f, -25.0f});
	camera.set_line(0.1f, 500.0f);
	camera.set_screen(SCREEN_X/2.0, SCREEN_Y);

	// prepare
	OBJECT *player = container.add_obj({0.0f, 2.5f, 0.0f}, 0, 2.5);
	player->hp = 100;
	
	ENEMY_MANAGE enemies(&container, player);
	
	GAUGE gauge(player, 1.0);
	CONTROLLER controller(&camera, player);
	
	make_wall(container);
	
	while (1)
	{
		renderer.clear();
		
		if (elapsed_time > frame_duration)
		{
			controller.control();
			
			container.update();
			enemies.manage();
			enemies.spawner();
			
			if (player->del)
			  system("cls"), exit(123);
			
			container.delete_obj();
			
			camera.prepare_rendering();
			
			for (auto &obj : container.all_objs)
			{
				if (abs(camera.pos.dist(obj->center)) < 150.0f)
				  camera.prepare_objects(obj->tris);
			}
			
			camera.sort_z();
			camera.screen_clipping(renderer);
			
			renderer.window();
			
			start_time = std::chrono::high_resolution_clock::now();
		}

	    end_time = std::chrono::high_resolution_clock::now();
	    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	}
	
	return 0;
}
















