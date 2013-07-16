#pragma once
#include "line.h"
#include "Circle.h"
#include "Vector2f.h"
#include "shared_struct.h"
#include <vector>
using namespace std;
class QT_square;
//http://gamedev.tutsplus.com/tutorials/implementation/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space/

#define Max_objects 3
#define max_tree_lvl 4


class QT_square //quad tree
{

public:
	void insert(Circle *c);
	void insert(line *l);
	void remove_objects();
	int get_lvl();
	void generate_possible_list(std::vector<Circle*> *inputlist, Circle *c);
	void generate_possible_list(std::vector<line*> *inputlist, Circle *c);
	QT_square(int lvl, AABB Bbox);
	QT_square(void);
	~QT_square(void);
	void Render();

private:
	int get_index(AABB &box);
	void generate_new_nodes();
	int tree_lvl;
	vector<Circle*> Cs;
	vector<line*> Ls;
	float width;
	float height;
	AABB bounding_box;
	QT_square *child_node[4];
	
};

