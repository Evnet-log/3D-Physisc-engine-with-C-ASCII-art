#ifndef GAME_H
#define GAME_H

class GAUGE
{
	public:
		OBJECT *obj;
		
		float max_num;
	
	public:
		GAUGE() = default;
		GAUGE(OBJECT *obj, float max_num) :obj(obj), max_num(max_num) {}
		
		Vector2d get_gauge(float spd)
		{
			float radian = -M_PI * (spd / max_num);
			
			Vector2d v;
			
			v.x = 1.0 * std::cos(radian) - 0.0 * std::sin(radian);
			v.y = 0.0 * std::cos(radian) + 1.0 * std::sin(radian);
			
			return {v.y, v.x};
		}
		
		void print(RENDERER *renderer)
		{
			float spd = (obj->center - obj->last_center).dist();
			
			Vector2d g1 = get_gauge(spd) * 8.0f;
			
			renderer->draw_circle({28.0, 61.0}, 10.0, ':');
			renderer->draw_line({28.0, 61.0}, Vector2d{28.0, 61.0}+g1, '.');
		}
};

class CONTROLLER
{
	public:
		CAMERA *camera;
		OBJECT *obj;
		
		float radian{0.0};
	
	public:
		CONTROLLER() = default;
		CONTROLLER(CAMERA *camera, OBJECT *obj) :camera(camera), obj(obj) {}
		
		void control()
		{
			if (GetAsyncKeyState(VK_LEFT))
			  radian -= 0.03f;
			if (GetAsyncKeyState(VK_RIGHT))
			  radian += 0.03f;

			Vector2d camera_rad = {std::cos((M_PI + M_PI/2.0f) - radian), std::sin((M_PI + M_PI/2.0f) - radian)};
			camera_rad *= 32.0;
			
			camera->set_pos(obj->center + Vector3d{camera_rad.x, 10.0, camera_rad.y});
			camera->turn_right(-radian);
			
			if (GetAsyncKeyState('W'))
			  obj->add_accel(camera->get_vector(0) * 600.0);
			if (GetAsyncKeyState('S'))
			  obj->add_accel(camera->get_vector(M_PI) * 600.0);
			if (GetAsyncKeyState('D'))
			  obj->add_accel(camera->get_vector(-M_PI/2.0) * 600.0);
			if (GetAsyncKeyState('A'))
			  obj->add_accel(camera->get_vector(M_PI/2.0) * 600.0);
		}
};

class ENEMY_MANAGE
{
	public:
		CONTAINER *container;
		OBJECT *target;
		
		std::vector<OBJECT*> enemies;
	
	public:
		ENEMY_MANAGE() = default;
		ENEMY_MANAGE(CONTAINER *container, OBJECT *target) :container(container), target(target) {}
		
		void add_enemy(Vector3d pos, float size)
		{
			OBJECT *obj = container->add_obj(pos, 0, size);
			enemies.push_back(obj);
			
			obj->hp = 5.0 * (size / 2.0);
		}
		
		void spawner()
		{
			if (enemies.size() == 0)
			{
				float size = (rand()%5 * 0.2) + 1.5;
				int count = rand()%3 + 1;
				
				for (int n = 0; n < count; ++n)
				{
					float x = (rand()%80 - 40.0);
					float z = (rand()%80 - 40.0);
					
					add_enemy({x, 20.0, z}, size);
				}
			}
		}
		
		void manage()
		{
			enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const OBJECT *target){
				return target->del;
			}), enemies.end());
			
			for (auto &ene : enemies)
			{
				Vector3d accel = (target->center - ene->center).normalise();
				ene->add_accel(accel*400.0);
			}
		}
};

void make_wall(CONTAINER &container)
{
	float radius = 60.0;
	float radian = M_PI*2 / 10.0;
	
	for (int n = 0; n < 10; ++n)
	{
		float cos_v = std::cos(radian*n) * radius;
		float sin_v = std::sin(radian*n) * radius;
		
		container.add_obj({cos_v, 0.0, sin_v}, 1, 1.5)->fix = true;
	}
}











#endif









