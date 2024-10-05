#ifndef THING_H
#define THING_H

void c()
{
  CONSOLE_CURSOR_INFO cursorInfo = { 0, };

  cursorInfo.dwSize = 1;

  cursorInfo.bVisible = FALSE;

  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y)
{

    COORD Pos;
    
    Pos.X = x-1;
    Pos.Y = y-1;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}

template <int N>
struct Mat
{
	public:
		std::vector<std::vector<double>> mat;
	
	public:
		Mat() { mat.resize(N, std::vector<double>(N, 0.0)); }
};

struct Vector4d
{
	public:
		double x, y, z, w;
	
	public:
		Vector4d() = default;
		Vector4d(double x, double y, double z, double w) :x(x), y(y), z(z), w(w) {}
};

struct Vector3d
{
	public:
		double x;
		double y;
		double z;
	
	public:
		Vector3d() = default;
		Vector3d(double x, double y, double z) :x(x), y(y), z(z) {}

		Vector3d& operator*=(const Vector3d &target) {
			x *= target.x, y *= target.y, z *= target.z;
			return *this;
		}
		
		Vector3d& operator+=(const Vector3d &target) {
			x += target.x, y += target.y, z += target.z;
			return *this;
		}
		
		Vector3d& operator-=(const Vector3d &target) {
			x -= target.x, y -= target.y, z -= target.z;
			return *this;
		}
		
		bool operator==(const Vector3d &target) const {
			return std::tie(x, y, z) == std::tie(target.x, target.y, target.z);
		}
		
		bool operator<(const Vector3d &target) const
		{
			return std::tie(x, y, z) < std::tie(target.x, target.y, target.z);
		}

		Vector3d operator*(const Vector3d &target) const {
			return {x * target.x, y * target.y, z * target.z};
		}
		
		Vector3d operator+(const Vector3d &target) const {
			return {x + target.x, y + target.y, z + target.z};
		}
		
		Vector3d operator-(const Vector3d &target) const {
			return {x - target.x, y - target.y, z - target.z};
		}
		
		Vector3d& operator*=(double f) {
			x *= f, y *= f, z *= f;
			return *this;
		}
		
		Vector3d operator*(double f) const {
			return {x * f, y * f, z * f};
		}
		
		double dot_product(const Vector3d &target) const {
			return x * target.x + y * target.y + z * target.z;
		}
		
		Vector3d cross_product(const Vector3d &target) const
		{
			Vector3d v {
				y * target.z - z * target.y,
				z * target.x - x * target.z,
				x * target.y - y * target.x
			};
			
			return v;
		}
		
		Vector3d normalise()
		{
			Vector3d v = *this;
			v *= (1.0f / sqrtf(x*x + y*y + z*z));
			
			return v;
		}
		
		double dist(const Vector3d &target = {0.0, 0.0, 0.0}) const
		{
			Vector3d v = *this - target;
			
			return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
		}
		
		void rotate_x(double radian, Vector3d center={0.0, 0.0, 0.0})
		{
			double z_ = (z-center.z) * std::cos(radian) - (y-center.y) * std::sin(radian);
			double y_ = (y-center.y) * std::cos(radian) + (z-center.z) * std::sin(radian);
			
			z = z_ + center.z;
			y = y_ + center.y;
		}
		
		void rotate_y(double radian, Vector3d center={0.0, 0.0, 0.0})
		{
			double x_ = (x-center.x) * std::cos(radian) - (z-center.z) * std::sin(radian);
			double z_ = (z-center.z) * std::cos(radian) + (x-center.x) * std::sin(radian);
			
			x = x_ + center.x;
			z = z_ + center.z;
		}
		
		void rotate_z(double radian, Vector3d center={0.0, 0.0, 0.0})
		{
			double x_ = (x-center.x) * std::cos(radian) - (y-center.y) * std::sin(radian);
			double y_ = (y-center.y) * std::cos(radian) + (x-center.x) * std::sin(radian);
			
			x = x_ + center.x;
			y = y_ + center.y;
		}
		
		void print() const
		{
			printf("%.20lf %.20lf %.20lf\n", x, y, z);
		}
};

template <typename T>
struct is_Vector3d : std::false_type {};

template <>
struct is_Vector3d<Vector3d> : std::true_type {};
template <>
struct is_Vector3d<Vector3d*> : std::true_type {};

struct Vector2d
{
	public:
		double x;
		double y;
	
	public:
		Vector2d() = default;
		Vector2d(int x, int y) :x(static_cast<double>(x)), y(static_cast<double>(y)) {}
		Vector2d(double x, double y) :x(x), y(y) {}
		Vector2d(const Vector2d &v)
		{
			x = v.x;
			y = v.y;
		}
		
		Vector2d& operator+=(const Vector2d &target) {
			x += target.x;
			y += target.y;
			
			return *this;
		}
		
		Vector2d& operator-=(const Vector2d &target) {
			x -= target.x;
			y -= target.y;
			
			return *this;
		}
		
		Vector2d& operator*=(double val) {
			x *= val;
			y *= val;
			
			return *this;
		}
		
		Vector2d operator+(const Vector2d &target) const {
			return {x + target.x, y + target.y};
		}
		
		Vector2d operator-(const Vector2d &target) const {
			return {x - target.x, y - target.y};
		}
		
		Vector2d operator*(double val) const {
			return {x * val, y * val};
		}
		
		bool operator==(const Vector2d &target) const {
			return x == target.x && y == target.y;
		}
		
		double dist()
		{
			return sqrt(x*x + y*y);
		}
};

struct Point;

template <typename T>
struct EDGE
{
	public:
		T p1;
		T p2;
	
	public:
		EDGE() = default;
		EDGE(T p1, T p2) :p1(p1), p2(p2) {}
		
		bool operator==(const T &target) const {
			return p1 == p1 && p2 == p2 || p1 == p2 && p2 == p1;
		}
};

template <>
struct EDGE<Point*>
{
	public:
		Point* p1;
		Point* p2;
	
	public:
		bool operator<(const EDGE<Point*> &target) const {

	        Point* first = std::min(p1, p2);
	        Point* second = std::max(p1, p2);
	        
			Point* target_first = std::min(target.p1, target.p2);
			Point* target_second = std::max(target.p1, target.p2);
	
	        if (first != target_first) {
	            return first < target_first;
	        }
	        return second < target_second;
		}
};

template <typename T>
struct TRIANGLE
{
	public:
		T p[3];
		
		int color;
		double volume;
	
	public:
		TRIANGLE() = default;
		TRIANGLE(T p1, T p2, T p3) :p{p1, p2, p3}, color(0), volume(0.0) {}
};

struct COOR
{
	public:
		int x;
		int y;
	
	public:
		COOR() = default;
		COOR(int x, int y) :x(x), y(y) {}
		COOR(double x, double y) { this->x = (int)round(x), this->y = (int)round(y); }
		COOR(Vector2d &v)  { x = (int)round(v.x), y = (int)round(v.y); }
		
		bool operator==(const COOR &target) const {
			return x == target.x && y == target.y;
		}
		
		bool operator<(const COOR &target) const {
			return y < target.y ? 1 : (y == target.y ? x < target.x : 0);
		}
};

std::vector<COOR> bresenham_line(COOR a, COOR b)
{
	int mx = a.x < b.x ? 1 : -1;
	int my = a.y < b.y ? 1 : -1;
	
	int width = abs(b.x - a.x);
	int height = abs(b.y - a.y);
	
	int val = width - height;
	
	std::vector<COOR> coors;
	
	while (1)
	{
		coors.emplace_back(a);
		
		if (a == b)
		  break;
		
		int ex = 2*val;
		
		if (ex > -height)
		{
			a.x += mx;
			val -= height;
		}
		
		if (ex < width)
		{
			a.y += my;
			val += width;
		}
	}
	
	return coors;
}

Vector4d matrix_multiple(Mat<4> &m, Vector3d &target)
{
	Vector4d v;
	
	v.x = target.x * m.mat[0][0] + target.y * m.mat[0][1] + target.z * m.mat[0][2] + 1.0f * m.mat[0][3];
	v.y = target.x * m.mat[1][0] + target.y * m.mat[1][1] + target.z * m.mat[1][2] + 1.0f * m.mat[1][3];
	v.z = target.x * m.mat[2][0] + target.y * m.mat[2][1] + target.z * m.mat[2][2] + 1.0f * m.mat[2][3];
	v.w = target.x * m.mat[3][0] + target.y * m.mat[3][1] + target.z * m.mat[3][2] + 1.0f * m.mat[3][3];
	
	return v;
}

struct Point
{
	public:
		Vector3d pos = {0.0f, 0.0f, 0.0f};
		Vector3d last_pos{0.0f, 0.0f, 0.0f};
		
		Vector3d accel{0.0f, 0.0f, 0.0f};
		
		double radius{0.0f};
		
	
	public:
		Point() = default;
		Point(Vector3d pos, double radius) :pos(pos), last_pos(pos), radius(radius) {}
		
		void update(double dt)
		{
			Vector3d velocity = pos - last_pos;
			Vector3d acc = accel * 0.5 * dt * dt;
			
			last_pos = pos;
			pos += velocity + acc;
			
			accel = {0.0f, 0.0f, 0.0f};
		}
		
		void add_vel(const Vector3d &vel)
		{
			last_pos -= vel;
		}
		
		void add_accel(const Vector3d &vel)
		{
			accel += vel;
		}
};

struct LINKED
{
	public:
		Point *p1;
		Point *p2;
		
		double dist;
	
	public:
		LINKED() = default;
		LINKED(Point *p1, Point *p2, double dist) :p1(p1), p2(p2), dist(dist) {}
		
		bool operator==(const LINKED &target) const {
			return p1->pos == target.p1->pos && p2->pos == target.p2->pos || p1->pos == target.p2->pos && p2->pos == target.p1->pos;
		}
};

struct BENDING
{
	public:
		Vector3d *a;
		Vector3d *b;
		Vector3d *c;
		Vector3d *d;
		
		float radian;
		bool dir;
	
	public:
		BENDING() = default;
		BENDING(Vector3d *a, Vector3d *b, Vector3d *c, Vector3d *d, float radian, bool dir) :a(a), b(b), c(c), d(d), radian(radian), dir(dir) {}
};

struct OBJECT
{
	public:
		std::vector<Point*> points;
		std::vector<LINKED> linkes;
		std::vector<BENDING> bends;
		std::vector<TRIANGLE<Vector3d*>> tris;
		
		Vector3d max;
		Vector3d min;
		
		Vector3d center;
		Vector3d last_center;
		
		Vector3d death_sign;
		
		float hp{5.0};
		float volume;
		float size;
		
		bool live{false};
		
		clock_t live_start;
		clock_t live_time;
		
		bool del{false};
		bool fix{false};
		bool coll{true};
	
	public:
		OBJECT() = default;
		
		void update()
		{
			max.x = std::numeric_limits<double>::lowest();
			max.y = std::numeric_limits<double>::lowest();
			max.z = std::numeric_limits<double>::lowest();
			
			min.x = std::numeric_limits<double>::max();
			min.y = std::numeric_limits<double>::max();
			min.z = std::numeric_limits<double>::max();
			
			center = {0.0, 0.0, 0.0};
			
			for (const auto &po : points)
			{
				center += po->pos;
				
				max.x = std::max(max.x, po->pos.x);
				max.y = std::max(max.y, po->pos.y);
				max.z = std::max(max.z, po->pos.z);
				
				min.x = std::min(min.x, po->pos.x);
				min.y = std::min(min.y, po->pos.y);
				min.z = std::min(min.z, po->pos.z);
			}
			
			center *= 1.0 / static_cast<double>(points.size());
		}
		
		bool overlap(const OBJECT &target)
		{
			if (min.x <= target.max.x && target.min.x <= max.x)
			{
				if (min.y <= target.max.y && target.min.y <= max.y)
				{
					if (min.z <= target.max.z && target.min.z <= max.z)
					  return true;
				}
			}
			
			return false;
		}
		
		void add_accel(Vector3d accel)
		{
			for (auto &po : points)
			  po->add_accel(accel);
		}
		
		void apply_dmg(Vector3d dmg, Vector3d sign)
		{
			hp -= dmg.dist() * 10.0;
			
			if (hp < 0.0)
			{
				death_sign = sign;
				death_sign.y = 0.0;
			}
		}
		
		void set_live_time(int alive)
		{
			live = true;
			live_start = clock();
			live_time = alive;
		}
};

void load_object_file(const char *fname, std::vector<Point*> &points, std::vector<LINKED> &linkes, std::vector<BENDING> &bends, std::vector<TRIANGLE<Vector3d*>> &tris, float size=1.0f)
{
    FILE *file = fopen(fname, "r");
    
    if (file == NULL)
      perror("Failed to open file");

    char j, e;
    double x, y, z;
    
    std::set<EDGE<Point*>> edges;
    std::map<std::pair<int, int>, std::vector<std::vector<int>>> bending;
    
    while (!feof(file))
	{
		fscanf(file, "%c %lf %lf %lf", &j, &x, &y, &z);
        while ((e = fgetc(file)) != EOF && e != '\n');
		
		if (j == 'v')
		  points.push_back(new Point{Vector3d{x*size, y*size, z*size}, 0.2});
		
		else if (j == 'f')
		{
			std::vector<int> vals = {static_cast<int>(x)-1, static_cast<int>(y)-1, static_cast<int>(z)-1};
			
			Point *p1 = points[vals[0]];
			Point *p2 = points[vals[1]];
			Point *p3 = points[vals[2]];
			
			tris.push_back(TRIANGLE<Vector3d*>(&p1->pos, &p2->pos, &p3->pos));
			
			edges.insert(EDGE<Point*>{p1, p2});
			edges.insert(EDGE<Point*>{p2, p3});
			edges.insert(EDGE<Point*>{p3, p1});
			
			for (int n = 0; n < 3; ++n)
			{
				for (int m = n+1; m < 3; ++m)
				{
					std::pair<int, int> pa = {vals[n], vals[m]};
					
					if (vals[n] < vals[m])
					  std::swap(pa.first, pa.second);
					
					bending[pa].push_back(vals);
				}
			}
		}
    }
	
	for (auto &pair : bending)
    {
		Vector3d *a = &points[pair.first.first]->pos;
		Vector3d *b = &points[pair.first.second]->pos;
		Vector3d *c = nullptr;
		Vector3d *d = nullptr;
    	
    	if (pair.second.size() == 2)
    	{
	    	for (auto &se : pair.second)
	    	{
	    		for (auto &n : se)
	    		{
	    			Vector3d *t1 = &points[n]->pos;
	    			
	    			if (a != t1 && b != t1)
	    			{
	    				if (c == nullptr)
	    				  c = t1;
	    				else
	    				  d = t1;
					}
				}
			}
			
			Vector3d ab = (*b - *a).normalise();
			Vector3d ac_nor = ab.cross_product({*c - *a}).normalise();
			Vector3d ad_nor = ab.cross_product({*d - *a}).normalise();
			
			float dot = ac_nor.dot_product(ad_nor);
			float radian = std::acos(dot);
			
			Vector3d cross = ac_nor.cross_product(ad_nor);
			bool dir = false;
			
			if (cross.dot_product(ab) > 0.0)
			  dir = true;
			
			bends.emplace_back(BENDING{a, b, c, d, radian, dir});
		}
	}
    
    for (auto &ed : edges)
    {
    	double dist = ed.p1->pos.dist(ed.p2->pos);
    	
    	linkes.emplace_back(LINKED{ed.p1, ed.p2, dist});
    }
	
    fclose(file);
}

Vector3d rodrigues_rotate(Vector3d vec, Vector3d axis, float radian)
{
	float cos_val = std::cos(radian);
	float sin_val = std::sin(radian);
	
	axis = axis.normalise();
	
	Vector3d v = axis * axis.dot_product(vec);
	
	Vector3d x_a = vec - v;
	Vector3d y_a = x_a.cross_product(axis);
	
	x_a *= cos_val;
	y_a *= sin_val;
	
	v += x_a;
	v += y_a;
	
	return v;
}

#endif










