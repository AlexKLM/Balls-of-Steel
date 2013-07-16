#pragma once
#include "Vector2f.h"
#include "RopePts.h"
#include "spring.h"
#include <concurrent_vector.h>
using namespace std;
using namespace Concurrency;

class Rope
{
private:
	concurrent_vector<RopePts>* rPts;
	std::vector<spring> springs;
public:
	void setup(Vector2f EndPt1, Vector2f EndPt2, int num_points, float desired_size, float gap);
	void debug_setup();
	void update(float dt);
	void reset();
	void calculate_physics(float dt);
	concurrent_vector<RopePts>* get_RopePts(); //return list of rope points for collisions detections
	Rope(void);
	~Rope(void);
};

