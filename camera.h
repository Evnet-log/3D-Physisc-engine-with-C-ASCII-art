#ifndef CAMERA_H
#define CAMERA_H

//"...-',^;<+!rc*/zsvCf3tuoywqkPVOGbUAKXH0MNWQ%&#@";
//"........---------+++++++++zzzzzzwwwwwwwwkkkkkkkPPPPVVVV####@@@@"

class CAMERA
{
	public:
		std::vector<TRIANGLE<Vector3d>> screen_tris;

	public:
		const char *COLOR = "...-,;<+!*/vftuoywqkPVOGbUAKXH0MNWQ%&#@";
		
		Vector3d target_pos{0.0f, 0.0f, 1.0f};
		Vector3d pos{0.0f, 0.0f, 0.0f};
		
		Vector3d up, forward, right;
		
		double near_p{1.0f};
		double far_p{500.f};
		
		double fov{M_PI/2.0f};
		
		double width{50};
		double height{50};
				
		Mat<4> perspective_mat;
		Mat<4> view_mat;
	
	public:
		CAMERA() = default;
		
		void set_pos(Vector3d pos)
		{
			this->pos = pos;
			this->target_pos = pos;
			
			target_pos.z += 1.0f;
		}
		
		void set_line(double near_p, double far_p)
		{
			this->near_p = near_p;
			this->far_p = far_p;
		}
		
		void set_fov(double fov)
		{
			this->fov = fov;
		}
		
		void set_screen(double width, double height)
		{
			this->width = width;
			this->height = height;
		}
		
		void move_front(double num)
		{
			Vector3d v = target_pos - pos;
			v = v.normalise();
			
			v *= num;
			
			pos += v;
			target_pos += v;
		}
		
		void move_right(double num)
		{
			Vector3d v = target_pos - pos;
			v = v.normalise();
			v.rotate_y(-M_PI/2.0f);
			
			v *= num;
			
			pos += v;
			target_pos += v;
		}
		
		void move_up(double num)
		{
			pos.y += num;
			target_pos.y += num;
		}
		
		void turn_right(double radian)
		{
			Vector3d v = target_pos - pos;
			v.rotate_y(radian);
			
			target_pos.x = pos.x + v.x;
			target_pos.z = pos.z + v.z;
		}
		
		Vector3d get_vector(float radian)
		{
			Vector3d v = target_pos - pos;
			v.rotate_y(radian);
			
			return v;
		}
		
		void sort_z()
		{
			std::sort(screen_tris.begin(), screen_tris.end(), [](const TRIANGLE<Vector3d> &t1, const TRIANGLE<Vector3d> &t2){
				double z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				double z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				
				return z1 > z2;
			});
		}
		
		void prepare_objects(std::vector<TRIANGLE<Vector3d*>> &object)
		{
			objects_rendering(object, view_mat, perspective_mat);
		}
		
		void prepare_rendering()
		{
			screen_tris.clear();
			make_basis();
				
			perspective_mat = make_perspective_mat();
			view_mat = make_view_change_mat();
		}
		
		void objects_rendering(std::vector<TRIANGLE<Vector3d*>> &object, Mat<4> &view_mat, Mat<4> &perspective_mat)
		{
			int tri_size = object.size();
			
			for (int m = 0; m < tri_size; ++m)
			{
				TRIANGLE<Vector3d> target{*(object[m].p[0]), *(object[m].p[1]), *(object[m].p[2])};
				target = view_change(view_mat, target);
				
				Vector3d normal = backface_culling(target);
				Vector3d check = target.p[0] + target.p[1] + target.p[2];
				check *= 1.0f / 3.0f;
				
				check = check.normalise();
				
				double dot_ratio = normal.dot_product(check);
				
				if (dot_ratio < 0.0f)
				{
					std::vector<TRIANGLE<Vector3d>> mini;
					clipping_triangle({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.1f}, target, mini);
					
					for (int n = 0; n < mini.size(); ++n)
					{
						TRIANGLE<Vector3d> t = perspective(perspective_mat, mini[n]);
						t.color = check_light(dot_ratio);
		
						for (int c = 0; c < 3; ++c)
						{
							t.p[c].x = (t.p[c].x + 1.0f) * 0.5f * width;
							t.p[c].y = (1.0f - t.p[c].y) * 0.5f * height;
						}
						
						screen_tris.push_back(t);
					}
				}
			}
		}
		
		Vector3d get_intersect_vec(Vector3d &plane_n, Vector3d &plane_p, Vector3d &line_start, Vector3d &line_end)
		{
			plane_n = plane_n.normalise();
			
			double d = plane_n.dot_product(plane_p);
			double start = plane_n.dot_product(line_start);
			double end = plane_n.dot_product(line_end);
			
			double ratio = (d - start) / (end - start);
			
			Vector3d v = line_end - line_start;
			v *= ratio;
			
			return v + line_start;
		}
		
		void screen_clipping(RENDERER &renderer)
		{
			for (auto &original_tri : screen_tris)
			{
				std::list<TRIANGLE<Vector3d>> tri_list = {original_tri};
				
				int count = 1;
				
				for (int n = 0; n < 4; ++n)
				{
					std::vector<TRIANGLE<Vector3d>> mini;
					
					while (count > 0)
					{
						TRIANGLE<Vector3d> target = tri_list.front();
						tri_list.pop_front();
						
						--count;
						
						if (n == 0)
						  clipping_triangle({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, target, mini);
						else if (n == 1)
						  clipping_triangle({0.0f, -1.0f, 0.0f}, {0.0f, (double)height-1.0f, 0.0f}, target, mini);
						else if (n == 2)
						  clipping_triangle({1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, target, mini);
						else if (n == 3)
						  clipping_triangle({-1.0f, 0.0f, 0.0f}, {(double)width-1.0f, 0.0f, 0.0f}, target, mini);
						
						for (int n = 0; n < mini.size(); ++n)
						  tri_list.push_back(mini[n]);
					}
					
					count = tri_list.size();
				}
				
				for (auto &tri : tri_list)
				{
					Vector2d vec[3];
					
					for (int n = 0; n < 3; ++n)
					  vec[n] = { tri.p[n].x, tri.p[n].y };
					
					renderer.fill_polygon({vec[0], vec[1], vec[2]}, original_tri.color);
					//renderer.draw_polygon({vec[0], vec[1], vec[2]}, '.');
				}
			}
		}
		
		void clipping_triangle(Vector3d plane_n, Vector3d plane_p, TRIANGLE<Vector3d> &tri, std::vector<TRIANGLE<Vector3d>> &reval)
		{
			auto dist = [&](const Vector3d &vec) {
				return plane_n.dot_product(vec) - plane_n.dot_product(plane_p);
			};
			
			std::vector<Vector3d> inside, outside;
			
			for (int n = 0; n < 3; ++n)
			  0 <= dist(tri.p[n]) ? inside.push_back(tri.p[n]) : outside.push_back(tri.p[n]);
			
			int ins = inside.size();
			int outs = outside.size();
			
			if (ins == 3) // all in
			  reval.push_back(tri);
			
			else if (ins == 1 && outs == 2) // mini tri
			{
				TRIANGLE<Vector3d> t1{ 
					inside[0], 
					get_intersect_vec(plane_n, plane_p, inside[0], outside[0]),
					get_intersect_vec(plane_n, plane_p, inside[0], outside[1])
				};
				
				reval.push_back(t1);
			}
			  
			else if (ins == 2 && outs == 1) // mini square(two mini tri)
			{
				TRIANGLE<Vector3d> t1{ 
					inside[0],
					inside[1],
					get_intersect_vec(plane_n, plane_p, inside[0], outside[0])
				};
				
				TRIANGLE<Vector3d> t2{ 
					inside[1],
					t1.p[2],
					get_intersect_vec(plane_n, plane_p, inside[1], outside[0])
				};
				
				reval.push_back(t1);
				reval.push_back(t2);
			}
		}
		
		Vector3d backface_culling(TRIANGLE<Vector3d> tri)
		{
			Vector3d normal, line_i, line_j;
			
			line_i = tri.p[1] - tri.p[0];
			line_j = tri.p[2] - tri.p[0];
			
			normal = line_i.cross_product(line_j);
			
			return normal.normalise();
		}
		
		char check_light(double light)
		{
			int color_n = static_cast<int>(round((-light) * 38.0f));
			
			return COLOR[color_n];
		}
		
		TRIANGLE<Vector3d> view_change(Mat<4> mat, TRIANGLE<Vector3d> &tris)
		{
		    Vector3d v[3];
		    
		    for (int n = 0; n < 3; ++n)
		    {
			    Vector4d v4 = matrix_multiple(mat, tris.p[n]);
			    v[n] = {v4.x, v4.y, v4.z};
			}
			
		    return TRIANGLE<Vector3d>(v[0], v[1], v[2]);
		}
		
		TRIANGLE<Vector3d> perspective(Mat<4> mat, TRIANGLE<Vector3d> &tris)
		{
		    Vector3d v[3];
		    
		    for (int n = 0; n < 3; ++n)
		    {
			    Vector4d v4 = matrix_multiple(mat, tris.p[n]);
			    v[n] = {v4.x, v4.y, v4.z};
			    
			    if (v4.w != 0.0f)
			      v[n] *= 1.0f / v4.w;
			}
		    
		    return TRIANGLE<Vector3d>(v[0], v[1], v[2]);
		}
		
		void make_basis()
		{
			forward = target_pos - pos;
			forward = forward.normalise();
			
			Mat<4> m = make_rotate_90();
			
			Vector4d v4 = matrix_multiple(m, forward);
			
			up = {0.0f, 1.0f, 0.0f};//{v4.x, v4.y, v4.z};
			
			right = up.cross_product(forward);
		}
		
		Mat<4> make_rotate_90()
		{
			Mat<4> m;
			
			m.mat[0][0] = 1;
			m.mat[1][2] = 1;
			m.mat[2][1] = -1;
			
			return m;
		}
		
		Mat<4> make_perspective_mat()
		{
			Mat<4> m;
			
			m.mat[0][0] = 1.0f / (std::tan(fov/2.0f) * (width/height));
			m.mat[1][1] = 1.0f / std::tan(fov/2.0f);
			m.mat[2][2] = far_p / (far_p - near_p);
			m.mat[2][3] = (-far_p * near_p) / (far_p - near_p);
			m.mat[3][2] = 1;
			
			return m;
		}
		
		Mat<4> make_view_change_mat()
		{
			Mat<4> m;
			
			m.mat[0] = {right.x, right.y, right.z, -pos.dot_product(right)};
			m.mat[1] = {up.x, up.y, up.z, -pos.dot_product(up)};
			m.mat[2] = {forward.x, forward.y, forward.z, -pos.dot_product(forward)};
			m.mat[3][2] = 1;
			
			return m;
		}
};







#endif

