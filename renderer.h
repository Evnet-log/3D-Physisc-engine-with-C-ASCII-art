#ifndef RENDERER_H
#define RENDERER_H

class RENDERER
{
	private:
		int M_X;
		int M_Y;
		
		char *print_map;
		char **map;
	
	public:
		RENDERER(int M_X, int M_Y)
		  :M_X(M_X), M_Y(M_Y)
		{
			print_map = new char[(M_X+1) * M_Y];
			map = new char*[M_Y];
			
			for (int n = 0; n < M_Y; ++n)
			{
				map[n] = print_map + n*(M_X+1);
				print_map[n*(M_X+1) + M_X] = '\n';
			}
			
			print_map[(M_X+1)*M_Y-1] = 0;
			
			clear();
		}
		
		void clear()
		{
			for (int y = 0; y < M_Y; ++y)
			{
				for (int x = 0; x < M_X; ++x)
				  map[y][x] = ' ';
			}
		}
		
		void draw_line(Vector2d p1, Vector2d p2, char c='.')
		{
			std::vector<COOR> coors = bresenham_line(p1, p2);
				
			for (const auto &coor : coors)
			{
				if (coor.x >= 0 && coor.x < (M_X)/2 && coor.y >= 0 && coor.y < M_Y)
				{
					map[coor.y][coor.x*2] = c;
					map[coor.y][coor.x*2+1] = c;
				}
			}
		}
		
		void draw_circle(Vector2d pos, double radius, char c='.')
		{
			std::vector<COOR> points;
			
			int x = static_cast<int>(round(pos.x));
			int y = static_cast<int>(round(pos.y));
			
			int count = 8;
			
			if (radius > 2)
			  count += (radius-2) * 8;
			
			double radian = (M_PI*2.0f)/static_cast<double>(count);
			
			for (int n = 0; n < count; ++n)
			{
				int px = x + static_cast<int>(round(std::cos(radian*n)*radius));
				int py = y + static_cast<int>(round(std::sin(radian*n)*radius));
				
				if (px >= 0 && px < (M_X)/2 && py >= 0 && py < M_Y)
				{
					map[py][px*2] = c;
					map[py][px*2+1] = c;
				}
					
				points.emplace_back(COOR{px, py});
			}
			
//			std::sort(points.begin(), points.end(), [](const COOR &a, const COOR &b){
//				return a.y < b.y;
//			});
//			
//			int up = points[0].y;
//			int down = points[points.size()-1].y;
//			
//			int start[down-up+1];
//			int end[down-up+1];
//			
//			for (int yy = up; yy <= down; ++yy)
//			{
//				start[yy-up] = std::numeric_limits<int>::max();
//				end[yy-up] = std::numeric_limits<int>::lowest();
//				
//				for (auto &po : points)
//				{
//					if (po.y == yy)
//					{
//						start[yy-up] = std::min(start[yy-up], po.x);
//						end[yy-up] = std::max(end[yy-up], po.x);
//					}
//				}
//			}
//			
//			for (int yy = up; yy <= down; ++yy)
//			{
//				for (int xx = start[yy-up] + 1; xx < end[yy-up]; ++xx)
//				{
//					if (xx >= 0 && xx < (M_X)/2 && yy >= 0 && yy < M_Y)
//					{
//						map[yy][xx*2] = '-';
//						map[yy][xx*2+1] = '-';
//					}
//				}
//			}
		}
		
		void draw_polygon(std::vector<Vector2d> polygon, char c='.')
		{
			int size = static_cast<int>(polygon.size());
			
			for (int n = 0; n < size; ++n)
			  draw_line(polygon[n], polygon[(n+1)%size]);
		}
		
		void fill_polygon(std::vector<Vector2d> polygon, char c='.')
		{
			int size = static_cast<int>(polygon.size());
			std::set<COOR> coors;
			
			for (int n = 0; n < size; ++n)
			{
				std::vector<COOR> get = bresenham_line(polygon[n], polygon[(n+1)%size]);
				coors.insert(get.begin(), get.end());
			}
			
			std::sort(polygon.begin(), polygon.end(), [](const Vector2d &a, const Vector2d &b){
				return a.y < b.y;
			});
			
			int min = round(polygon[0].y);
			int max = round(polygon[size-1].y);

			int start[max-min+1];
			int end[max-min+1];
			
			for (int n = 0; n < max-min+1; ++n)
			{
				start[n] = std::numeric_limits<int>::max();
				end[n] = std::numeric_limits<int>::lowest();
			}
			
			for (const auto &coor : coors)
			{
				start[coor.y - min] = std::min(start[coor.y - min], coor.x);
				end[coor.y - min] = std::max(end[coor.y - min], coor.x);
			}
			
			for (int y = min; y <= max; ++y)
			{
				for (int x = start[y-min]; x <= end[y-min]; ++x)
				{
					if (x >= 0 && x < (M_X)/2 && y >= 0 && y < M_Y)
					{
						map[y][x*2] = c;
						map[y][x*2+1] = c;
					}
				}
			}
		}
		
		void window()
		{
			gotoxy(1,1);
			printf("%s", print_map);
		}
};













#endif
