#ifndef CONTAINER_H
#define CONTAINER_H

class CONTAINER
{
	public:
		std::vector<OBJECT*> all_objs;
		
		Vector3d accel;
		
		double dt;
		
		int repeat;
	
	public:
		CONTAINER() = default;
		
		void set_accel(Vector3d n) { accel = n; }
		void set_dt(double n) { dt = n; }
		void set_repeat(int n) { repeat = n; }
		
		OBJECT *add_obj(const Vector3d &pos, int type, float size=1.0)
		{
			OBJECT *object = new OBJECT;
			object->size = size;
			
			if (type == 0)
			  load_object_file("Cube.obj", object->points, object->linkes, object->bends, object->tris, size);
			else
			  load_object_file("Tetra.obj", object->points, object->linkes, object->bends, object->tris, size);
			
			object->update();
			
			float all_volume = 0.0;
			
			for (auto &tri : object->tris)
			{
				Vector3d a = *tri.p[0] - object->center;
				Vector3d b = *tri.p[1] - object->center;
				Vector3d c = *tri.p[2] - object->center;
				
				tri.volume = a.cross_product(b).dot_product(c) * (1.0/6.0);
				all_volume += tri.volume;
			}
			
			object->volume = all_volume;
			
			for (auto &po : object->points)
			  po->pos += pos, po->last_pos += pos;
			
			all_objs.push_back(object);
			
			return object;
		}
		
		void set_gravity()
		{
			Vector3d target{0.0f, 10.0f, 0.0f};
			
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &po : obj->points)
					  po->add_accel(accel);
				}
			}
		}
		
		void obj_update()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &po : obj->points)
					  po->update(dt/repeat);
					
					obj->last_center = obj->center;
					obj->update();
				}
			}
		}
		
		void floor_constraint()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &po : obj->points)
					{
						if (po->pos.y < 0.0f)
						{
							po->pos.y = 0.0f;
							
							po->last_pos.x += (po->pos.x - po->last_pos.x)*0.5*0.5;
							po->last_pos.z += (po->pos.z - po->last_pos.z)*0.5*0.5;
						}
					}
				}
			}
		}
		
		void spring_constraint()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &link : obj->linkes)
					{
						Vector3d v = link.p2->pos - link.p1->pos;
						double dist = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
						
						double m = (dist - link.dist) / dist;
						v *= m * 2000.0;
						
						Vector3d s1 = v;
						Vector3d s2 = v * -1.0;
						
						link.p1->add_accel(s1);
						link.p2->add_accel(s2);
					}
				}
			}
		}
		
		void volume_constraint()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					obj->update();
					
					for (auto &tri : obj->tris)
					{
						Vector3d a = *tri.p[0] - obj->center;
						Vector3d b = *tri.p[1] - obj->center;
						Vector3d c = *tri.p[2] - obj->center;
						
						double vol_ = a.cross_product(b).dot_product(c) * (1.0/6.0);
						
						if (vol_ > 1e-3)
						{
							double ratio = 1.0 - (vol_ / tri.volume);
							
							*tri.p[0] += (a * ratio) * dt * dt * 0.1;
							*tri.p[1] += (b * ratio) * dt * dt * 0.1;
							*tri.p[2] += (c * ratio) * dt * dt * 0.1;
						}
					}
				}
			}
		}
		
		void bending_constraint()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &bend : obj->bends)
					{
						Vector3d ab = (*bend.b - *bend.a).normalise();
						Vector3d ac_nor = ab.cross_product(*bend.c - *bend.a).normalise();
						Vector3d ad_nor = ab.cross_product(*bend.d - *bend.a).normalise();
						
						Vector3d cross = ac_nor.cross_product(ad_nor);
						
						float dot = ac_nor.dot_product(ad_nor);
						float cur_radian = std::acos(dot);
						float dir_check = cross.dot_product(ab);
						
						if (dir_check > 0.0 && !bend.dir || dir_check < 0.0 && bend.dir)
						  cur_radian = M_PI*2.0 - cur_radian;
						
						float dif = cur_radian - bend.radian;
						
						if (std::abs(dif) > 0.0)
						{
							if (bend.dir)
							  dif *= -1.0;
							
							Vector3d vec_c = *bend.c - *bend.a;
							Vector3d vec_d = *bend.d - *bend.a;
							
							Vector3d result_c = *bend.a + rodrigues_rotate(vec_c, ab, dif*0.5);
							Vector3d result_d = *bend.a + rodrigues_rotate(vec_d, ab, dif*-0.5);
							
							Vector3d d1 = (result_c - *bend.c) * dt * dt * 0.1;
							Vector3d d2 = (result_d - *bend.d) * dt * dt * 0.1;
							
							*bend.c += d1;
							*bend.d += d2;
						}
					}
				}
			}
		}
		
		void collision()
		{
			int size = all_objs.size();
			
			std::vector<Vector3d*> target1;
			std::vector<Vector3d*> target2;
			
			for (int n = 0; n < size; ++n)
			{
				if (!all_objs[n]->fix && all_objs[n]->coll)
				{
					target1.clear();
					
					for (const auto &po : all_objs[n]->points)
					  target1.emplace_back(&po->pos);
					
					for (int m = 0; m < size; ++m)
					{
						if (n != m && all_objs[m]->coll)
						{
							all_objs[n]->update();
							all_objs[m]->update();
							
							if (all_objs[n]->overlap(*all_objs[m]))
							{
								Vector3d dir{0.0, 0.0, 0.0};
								
								target2.clear();
								
								for (const auto &po : all_objs[m]->points)
								  target2.emplace_back(&po->pos);
								
								if (gjk(target1, target2, dir))
								{
									float ratio_1 = all_objs[n]->volume / (all_objs[n]->volume + all_objs[m]->volume);
									float ratio_2 = all_objs[m]->volume / (all_objs[n]->volume + all_objs[m]->volume);
									
									Vector3d vec1 = (all_objs[n]->center - all_objs[n]->last_center);
									Vector3d vec2 = (all_objs[m]->center - all_objs[m]->last_center);
									
									if (!all_objs[m]->fix)
									{	
										if (vec1.dist() > 1e-4 || vec2.dist() > 1e-4)
										{
											double dot1 = vec1.dot_product(dir);
											double dot2 = vec2.dot_product(dir*-1.0);
											
											if (dot1 > dot2)
											  all_objs[m]->apply_dmg(vec1, dir);
											else
											  all_objs[n]->apply_dmg(vec2, dir*-1.0);
										}
										
										for (auto &po : all_objs[n]->points)
										  po->pos -= dir*ratio_2;
										
										for (auto &po : all_objs[m]->points)
										  po->pos += dir*ratio_1;
									}
									
									else
									{
										for (auto &po : all_objs[n]->points)
										  po->pos -= dir;//po->add_accel(dir * -500.0);
										
										all_objs[n]->apply_dmg(vec1, dir*-1.0);
									}
								}
							}
						}
					}
				}
			}
		}
		
		void apply_resistance()
		{
			for (auto &obj : all_objs)
			{
				if (!obj->fix)
				{
					for (auto &po : obj->points)
					{
						Vector3d v = (po->pos - po->last_pos) * (1.0 / dt);
						v *= -2.5;
						
						po->add_accel(v);
					}
				}
			}	
		}
		
		void del_check()
		{
			int size = all_objs.size();
			
			for (int n = 0; n < size; ++n)
			{
				if (!all_objs[n]->fix)
				{
					if (all_objs[n]->hp <= 0.0)
					{
						for (int m = 0; m < 8; ++m)
						{
							OBJECT *target = add_obj(all_objs[n]->center, 0, all_objs[n]->size/2.0);
							target->coll = false;
							target->set_live_time(3000);
							
							float xx = (rand()%100 - 50.0) / 50.0;
							float zz = (rand()%100 - 50.0) / 50.0;
							float yy = rand()%50 / 50.0;
							
							Vector3d accel = all_objs[n]->death_sign.normalise() * 100000.0;
							accel += {100000.0*xx, 100000.0*(yy+0.5), 100000.0*zz};
							
							target->add_accel(accel);
						}
						
						all_objs[n]->del = true;
					}
					
					else if (all_objs[n]->live)
					{
						clock_t check = clock() - all_objs[n]->live_start;
						
						if (check > all_objs[n]->live_time)
						  all_objs[n]->del = true;
					}
					
					if (all_objs[n]->del)
					  all_objs[n]->fix = true;
				}
			}
		}
		
		void delete_obj()
		{
			std::set<OBJECT*> dels;
			
			for (auto &obj : all_objs)
			{
				if (obj->del)
				  dels.insert(obj);
			}

			all_objs.erase(std::remove_if(all_objs.begin(), all_objs.end(), [](const OBJECT *target){
				return target->del;
			}), all_objs.end());
		
			for (auto &del : dels)
			  delete del;
		}
		
		void update()
		{
			for (int n = 0; n < repeat; ++n)
			{
				set_gravity();
				
				floor_constraint();
				spring_constraint();
				volume_constraint();
				bending_constraint();
				
				apply_resistance();
				collision();
				
				obj_update();
				del_check();
			}
		}
};





#endif





