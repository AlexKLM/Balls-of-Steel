#include "QT_square.h"
#include <Windows.h>
#include <gl\gl.h>  
#define _USE_MATH_DEFINES
#include <math.h>

QT_square::QT_square(void)
{
}


QT_square::~QT_square(void)
{
}


QT_square::QT_square(int lvl, AABB Bbox)
{
	tree_lvl = lvl;
	bounding_box = Bbox;
	Vector2f pt1 = Vector2f(bounding_box.max.GetX(),0);
	Vector2f pt2 = Vector2f(bounding_box.min.GetX(),0);

	width = pt1.distance(pt2);
	pt1 = Vector2f(0,bounding_box.max.GetY());
	pt2 = Vector2f(0,bounding_box.min.GetY());
	height = pt1.distance(pt2);

	for(int i = 0; i < 4; i++)
	{
		child_node[i] = NULL;
	}
}

void QT_square::remove_objects()
{
	Cs.clear();
	for(int i = 0; i < 4; i++)
	{
		if(child_node[i] !=NULL)
		{
			child_node[i]->remove_objects();
			child_node[i] = NULL;
		}
	}
}

void QT_square::generate_new_nodes()
{
	float subWidth = width/2;
	float subHeight = height/2;
	float x = bounding_box.min.GetX();
	float y = bounding_box.min.GetY();

	AABB box;
	box.min = Vector2f(x,y);
	box.max = Vector2f(x+subWidth, y + subHeight);

	child_node[0] = new QT_square(tree_lvl+1,box);

	box.min = Vector2f(x,y+subHeight);
	box.max = Vector2f(x+subWidth,y + height);

	child_node[1] = new QT_square(tree_lvl+1,box);

	box.min = Vector2f(x+subWidth,y+subHeight);
	box.max = Vector2f(x+width,y + height);

	child_node[2] = new QT_square(tree_lvl+1,box);

	box.min = Vector2f(x+subWidth,y);
	box.max = Vector2f(x+width,y + subHeight);

	child_node[3] = new QT_square(tree_lvl+1,box);
}

int QT_square::get_index(AABB &box)
{
	int index = -1;
	float Xmid = bounding_box.min.GetX() + (width/2);
	float Ymid = bounding_box.min.GetY() + (height/2);


	//child_nodes pos
	//       x mid
	//----------------
	//|      |		 |
	//|  1   |   2   |
	//---------------- y mid
	//|  0   |  3    |
	//|      |       |
	//----------------
	//check if object is on above or below the midpoint

	bool lower_half = (box.min.GetY() < Ymid && box.max.GetY() < Ymid);
	bool top_half = (box.min.GetY() > Ymid);

	//figure out if it can be fit (completety) into any smaller box
	if(box.min.GetX() < Xmid && box.max.GetX() < Xmid) //check if its left side
	{
		if(top_half)
		{
			index = 1;
		}
		else if(lower_half)
		{
			index = 0;
		}
	}
	else if(box.min.GetX() > Xmid) //check if it is in the right side
	{
		if(top_half)
		{
			index = 2;
		}
		else if(lower_half)
		{
			index = 3;
		}
	}

	return index; //if none of these pass, it will not be put into this square
}

void QT_square::insert(Circle *c)
{
	if(child_node[0] != NULL)
	{
		int index = get_index(c->get_Bbox_static());
		if(index != -1)
		{
			child_node[index]->insert(c);
			return;
		}
	}
	Cs.push_back(c);

	if(Cs.size() + Ls.size()> Max_objects && tree_lvl < max_tree_lvl)
	{
		if(child_node[0] == NULL)
		{
			generate_new_nodes();
		}
		int i = 0;
		while(i < Cs.size())
		{
			int index = get_index(Cs[i]->get_Bbox_static());
			if(index != -1)
			{
				child_node[index]->insert(Cs[i]);
				Cs.erase(Cs.begin() + i);
			}
			else
			{
				i++;
			}
		}
	}
}

void QT_square::insert(line *l)
{
	if(child_node[0] != NULL)
	{
		int index = get_index(l->get_Bbox());
		if(index != -1)
		{
			child_node[index]->insert(l);
			return;
		}
	}
	Ls.push_back(l);

	if(Ls.size() + Cs.size()> Max_objects && tree_lvl < max_tree_lvl)
	{
		if(child_node[0] == NULL)
		{
			generate_new_nodes();
		}
		int i = 0;
		while(i < Ls.size())
		{
			int index = get_index(Ls[i]->get_Bbox());
			if(index != -1)
			{
				child_node[index]->insert(Ls[i]);
				Ls.erase(Ls.begin() + i);
			}
			else
			{
				i++;
			}
		}
	}
}

void QT_square::generate_possible_list(std::vector<line*> *inputlist, Circle *c)
{
	int index ;
	
	if( child_node[0] != NULL)
	{
		index = get_index(c->get_Bbox_static());
		if(index != -1)
		{
			child_node[index]->generate_possible_list(inputlist,c);
		}
		
	}
	for(int i = 0; i < Ls.size(); i++)
	{
		//if(c->getID() != Cs[i]->getID())
		//{
			inputlist->push_back(Ls[i]);
		//}
	}
}


void QT_square::generate_possible_list(std::vector<Circle*> *inputlist, Circle *c)
{
	int index ;
	
	if( child_node[0] != NULL)
	{
		index = get_index(c->get_Bbox_static());
		if(index != -1)
		{
			child_node[index]->generate_possible_list(inputlist,c);
		}
		
	}
	for(int i = 0; i < Cs.size(); i++)
	{
		//if(c->getID() != Cs[i]->getID())
		//{
			inputlist->push_back(Cs[i]);
		//}
	}
}

void QT_square::Render() //for debug
{

	glBegin(GL_LINE_LOOP);
		glColor3d(0, 1, 1);
		glVertex2d(bounding_box.min.GetX(),bounding_box.min.GetY());
		glVertex2d(bounding_box.min.GetX(),bounding_box.max.GetY());
		glVertex2d(bounding_box.max.GetX(),bounding_box.max.GetY());
		glVertex2d(bounding_box.max.GetX(),bounding_box.min.GetY());
	glEnd();
	if(child_node[0] != NULL)
	{
		for(int i = 0; i < 4; i++)
		{
			child_node[i]->Render();
		}
	}
}