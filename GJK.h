#ifndef GJK_H
#define GJK_H

Vector3d support(std::vector<Vector3d*> &target1, std::vector<Vector3d*> &target2, Vector3d dir)
{
	Vector3d max_vec1;
	Vector3d max_vec2;
	
	double max = std::numeric_limits<double>::lowest();
	
	for (const auto &vec : target1)
	{
		float val = dir.dot_product(*vec);
		
		if (max < val)
		  max = val, max_vec1 = *vec;
	}
	
	max = std::numeric_limits<double>::lowest();
	dir *= -1.0;
	
	for (const auto &vec : target2)
	{
		float val = dir.dot_product(*vec);
		
		if (max < val)
		  max = val, max_vec2 = *vec;
	}
	
	return max_vec1 - max_vec2;
}

Vector3d origin_to_triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c)
{
	Vector3d check = a;
	
	Vector3d ab = b - a;
	Vector3d ac = c - a;
	
	Vector3d reval = ab.cross_product(ac);
	
	if (reval.dot_product(check) < 0.0f)
	  reval *= -1.0;
	
	return reval;
}

bool in_triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Vector3d &target)
{
	Vector3d ab = b - a;
	Vector3d ac = c - a;
	
	Vector3d bc = c - b;
	Vector3d ba = a - b;
	
	Vector3d ca = a - c;
	Vector3d cb = b - c;
	
	Vector3d ao = target - a;
	Vector3d bo = target - b;
	Vector3d co = target - c;
	
	Vector3d ab_cross = ab.cross_product(ab.cross_product(ac));
	Vector3d bc_cross = bc.cross_product(bc.cross_product(ba));
	Vector3d ca_cross = ca.cross_product(ca.cross_product(cb));
	
	return ab_cross.dot_product(ao) < 0.0f && bc_cross.dot_product(bo) < 0.0f && ca_cross.dot_product(co) < 0.0f;
}

Vector3d get_distance(const Vector3d &a, const Vector3d &b, const Vector3d &c)
{
	std::vector<Vector3d> dist;
	
	Vector3d result;
	float min_dist = std::numeric_limits<float>::max();
	
	dist.push_back(a);
	dist.push_back(b);
	dist.push_back(c);
	
	Vector3d dir = origin_to_triangle(a, b, c);
	dir = dir.normalise();
	
	float in_tri = dir.dot_product(a);
	dir *= in_tri;
	
	if (in_triangle(a, b, c, dir))
	  dist.push_back(dir);
	
	Vector3d ab = b - a;
	Vector3d bc = c - b;
	Vector3d ca = a - c;
	
	float ratio1 = ab.dot_product(a*-1.0) / ab.dot_product(ab);
	float ratio2 = bc.dot_product(b*-1.0) / bc.dot_product(bc);
	float ratio3 = ca.dot_product(c*-1.0) / ca.dot_product(ca);
	
	if (0 < ratio1 && ratio1 < 1)
	  dist.push_back(a + (ab*ratio1));
	
	if (0 < ratio2 && ratio2 < 1)
	  dist.push_back(b + (bc*ratio2));
	
	if (0 < ratio3 && ratio3 < 1)
	  dist.push_back(c + (ca*ratio3));
	
	for (const auto &target : dist)
	{
		float t_dist = target.dist();
		
		if (min_dist > t_dist)
		  result = target, min_dist = t_dist;
	}
	
	return result;
}

void check_line(std::vector<Vector3d> &simplex, Vector3d &dir)
{
	Vector3d a = simplex[1];
	Vector3d b = simplex[0];
	
	Vector3d ab = b - a;
	Vector3d ao = a * -1.0;
	
	dir = ab.cross_product(ao).cross_product(ab);
}

void check_triangle(std::vector<Vector3d> &simplex, Vector3d &dir)
{
	Vector3d a = simplex[2];
	Vector3d b = simplex[1];
	Vector3d c = simplex[0];
	
	Vector3d to_tri = origin_to_triangle(a, b, c);
	to_tri = to_tri.normalise();
	
	Vector3d in_tri = to_tri * to_tri.dot_product(a);
	
	Vector3d ab = b - a;
	Vector3d ac = c - a;
	
	Vector3d ca = a - c;
	Vector3d cb = b - c;
	
	Vector3d ao = in_tri - a;
	Vector3d co = in_tri - c;
	
	Vector3d ab_cross = ab.cross_product(ab.cross_product(ac));
	Vector3d ca_cross = ca.cross_product(ca.cross_product(cb));
	
	if (ab_cross.dot_product(ao) > 0.0)
	{
		simplex.erase(simplex.begin());
		dir = ab_cross;
		
		return;
	}
	
	if (ca_cross.dot_product(co) > 0.0)
	{
		simplex.erase(simplex.begin()+1);
		dir = ca_cross;
		
		return;
	}
	
	dir = to_tri * -1.0;
	
	return;
}

bool check_tetrahedron(std::vector<Vector3d> &simplex, Vector3d &dir)
{
	Vector3d a = simplex[3];
	Vector3d b = simplex[2];
	Vector3d c = simplex[1];
	Vector3d d = simplex[0];
	
	Vector3d abc = (a-b).cross_product(a-c);
	Vector3d acd = (a-c).cross_product(a-d);
	Vector3d adb = (a-d).cross_product(a-b);
	
	if (abc.dot_product(d-a) > 0.0)
	  abc *= -1.0f;
	if (acd.dot_product(b-a) > 0.0)
	  acd *= -1.0f;
	if (adb.dot_product(c-a) > 0.0)
	  adb *= -1.0f;
	
	Vector3d ao = a * -1.0;
	
	if (abc.dot_product(ao) > 0.0)
	{
		simplex.erase(simplex.begin());
		check_triangle(simplex, dir);
		
		return false;
	}
	
	if (acd.dot_product(ao) > 0.0)
	{
		simplex.erase(simplex.begin()+2);
		check_triangle(simplex, dir);
		
		return false;
	}
	
	if (adb.dot_product(ao) > 0.0)
	{
		simplex.erase(simplex.begin()+1);
		check_triangle(simplex, dir);
		
		return false;
	}
	
	return true;
}

Vector3d epa(std::vector<Vector3d*> &target1, std::vector<Vector3d*> &target2, std::vector<Vector3d> simplex);

bool gjk(std::vector<Vector3d*> &target1, std::vector<Vector3d*> &target2, Vector3d &dir)
{
	Vector3d direction = {1.0, 0.0, 0.0};
	
	std::vector<Vector3d> simplex = {support(target1, target2, direction)};
	std::set<Vector3d> points;
	
	int point_count = 0;
	int sycle = 0;
	
	direction = simplex[0] * -1.0;
	
	for (int count = 0; ; ++count)
	{
		if (count > 50)
		{
			system("cls");
			
			for (const auto &po : target1)
			  po->print();
			
			printf("\n");
			
			for (const auto &po : target2)
			  po->print();
			
			while (1) ;
		}
		
		simplex.push_back(support(target1, target2, direction));
		
		Vector3d new_one = simplex.back();
		points.insert(new_one);
		
		if (points.size() > point_count)
		  ++point_count;
		
		else
		{
			++sycle;
			
			if (sycle >= 2)
			  return false;
		}
		
		
		if (direction.dot_product(new_one) <= 0.0)
		  return false;
		
		int size = simplex.size();
		
		if (size == 2)
		  check_line(simplex, direction);
		
		else if (size == 3)
		  check_triangle(simplex, direction);
		
		else if (size == 4)
		{
			if (check_tetrahedron(simplex, direction))
			{
				dir = epa(target1, target2, simplex);
				
				return true;
			}
		}
	}
}

void find_min_direction(std::vector<TRIANGLE<Vector3d>> &triangles, TRIANGLE<Vector3d> &target_tri, Vector3d &min_dir)
{
	TRIANGLE<Vector3d> result;
	
	float min = std::numeric_limits<float>::max();
	
	for (const auto &tri : triangles)
	{
		Vector3d dir = get_distance(tri.p[0], tri.p[1], tri.p[2]);
		float val = dir.dist();
		
		if (min > val)
		  result = tri, min = val, min_dir = dir;
	}
	
	target_tri = result;
}

Vector3d get_out_dir(const Vector3d &a, const Vector3d &b, const Vector3d &c, std::vector<Vector3d> &simplex)
{
	Vector3d dir = origin_to_triangle(a, b, c);
	Vector3d val;
	
	float max = 0;
	
	for (const auto &vec : simplex)
	{
		if (a == vec || b == vec || c == vec)
		  continue;
		
		float dot = std::abs(dir.dot_product(vec - a));
		
		if (dot > max)
		  max = dot, val = vec - a;
	}
	
	if (dir.dot_product(val) > 0.0)
	  dir *= -1.0;
	
	return dir;
}

Vector3d epa(std::vector<Vector3d*> &target1, std::vector<Vector3d*> &target2, std::vector<Vector3d> simplex)
{
	std::vector<TRIANGLE<Vector3d>> triangles;
	std::vector<Vector3d> points;
	
	triangles.emplace_back(TRIANGLE<Vector3d>{simplex[0], simplex[1], simplex[2]});
	triangles.emplace_back(TRIANGLE<Vector3d>{simplex[3], simplex[0], simplex[1]});
	triangles.emplace_back(TRIANGLE<Vector3d>{simplex[3], simplex[1], simplex[2]});
	triangles.emplace_back(TRIANGLE<Vector3d>{simplex[3], simplex[2], simplex[0]});
	
	for (const auto &po : simplex)
	  points.emplace_back(po);
	
	for (int count = 0; ; ++count)
	{
		std::vector<TRIANGLE<Vector3d>> new_tris;
		TRIANGLE<Vector3d> target_tri;
		
		Vector3d direction{0.0, 0.0, 0.0};
		Vector3d dist{0.0, 0.0, 0.0};
		
		find_min_direction(triangles, target_tri, dist);
		direction = get_out_dir(target_tri.p[0], target_tri.p[1], target_tri.p[2], points);
		
		Vector3d point = support(target1, target2, direction);
		
		for (const auto &po : points)
		{
			if (po == point)
			{
				return dist;
			}
		}
		
		std::vector<bool> del_check(triangles.size(), false);
		
		for (auto &tri : triangles)
		{
			Vector3d dir = point - tri.p[0];
			Vector3d vec;
			
			vec = get_out_dir(tri.p[0], tri.p[1], tri.p[2], points);
			
			if (dir.dot_product(vec) > 0.0)
			{
				new_tris.emplace_back(TRIANGLE<Vector3d>{point, tri.p[0], tri.p[1]});
				new_tris.emplace_back(TRIANGLE<Vector3d>{point, tri.p[1], tri.p[2]});
				new_tris.emplace_back(TRIANGLE<Vector3d>{point, tri.p[2], tri.p[0]});
				
				del_check[&tri - &triangles[0]] = true;
			}
		}
		
		points.emplace_back(point);
		
		triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](const TRIANGLE<Vector3d>& tri){
			return del_check[&tri - &triangles[0]];
		}), triangles.end());
		
		del_check.clear();
		del_check.resize(new_tris.size(), false);
		
		for (auto n = new_tris.begin(); n != new_tris.end(); ++n)
		{
			for (auto m = n+1; m != new_tris.end(); ++m)
			{
				if (n->p[0] == m->p[0] && n->p[1] == m->p[1] && n->p[2] == m->p[2] ||
				    n->p[0] == m->p[1] && n->p[1] == m->p[2] && n->p[2] == m->p[0] || 
					n->p[0] == m->p[2] && n->p[1] == m->p[0] && n->p[2] == m->p[1] ||
					n->p[0] == m->p[0] && n->p[1] == m->p[2] && n->p[2] == m->p[1] ||
					n->p[0] == m->p[1] && n->p[1] == m->p[0] && n->p[2] == m->p[2] ||
					n->p[0] == m->p[2] && n->p[1] == m->p[1] && n->p[2] == m->p[0])
				{
					del_check[n-new_tris.begin()] = true;
					del_check[m-new_tris.begin()] = true;
				}
			}
		}
		
		new_tris.erase(std::remove_if(new_tris.begin(), new_tris.end(), [&](const TRIANGLE<Vector3d>& tri){
			return del_check[&tri - &new_tris[0]];
		}), new_tris.end());
		
		for (auto &tri : new_tris)
		  triangles.emplace_back(tri);
	}
}





#endif





