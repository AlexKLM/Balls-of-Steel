#pragma once
#include "ContactManifold.h"
#include "Vector2f.h"
#include "line.h"
#include "shared_struct.h"
#include "Grid.h"
#include <random>
#include <time.h> 
class QT_square;
class Circle
{
public:
	Circle(void);
	Circle(Vector2f Pos, float radius, float mass);
	~Circle(void);

	void CalculatePhysicsImE(float dt);
	void CalculatePhysicsImE(Vector2f V,float dt);
	void CalculatePhysicsRK(float dt);
	void CalculatePhysicsRK(Vector2f V, float dt);


	bool CollisionWithCircle(Circle* circle, float dt);
	bool CollisionWithLine(line* _line, float dt); 
	bool CollisionWithLineEnd(Vector2f P, float dt);
	void Update(float dt);

	void CollisionResponseWithCircle(ManifoldPoint &point,float dt);
	void CollisionResponseWithLine(ManifoldPoint &point,float dt);
	void CollisionResponseWithLineEnd(ManifoldPoint &point, float dt);

	void SetAirforce(Vector2f force);
	void SetPos(float x, float y);
	void SetNewPos(float x, float y);
	void SetVel(float x, float y);
	void SetNewVel(Vector2f vel);
	void SetMass(float mass);
	void SetFric(float input);
	void SetElas(float input);
	void SetCirElas(float input);

	bool will_collide();
	float get_contact_point();

	void Calculate_friction(Vector2f &normal, Vector2f &v, float e,bool add_to_net);


	void add_to_manifold(ContactManifold *contactManifold);
	AABB get_Bbox_static();

	Vector2f GetForce() const;
	Vector2f GetPos() const;
	Vector2f GetNewPos() const;
	Vector2f GetVel() const;
	Vector2f GetNewVel() const;
	float GetMass() const;
	void ResetPos();
	float GetRadius() const;
	
	float GetAngle() const;

	//float friction(float e, float dt);
	bool have_collided();
	//void Render() const;
	bool is_stationary();
	void set_stationary(bool input);
	int getID();
	void set_pos_col(bool input);

	void set_fixed(bool input);
	bool is_fixed();
	
	void add_force(float x, float y);
	void moveRK(float dt);
	void move(float dt);

	void containment();
private:
	bool fixed;//fixed circle cannot move(dirty code for rope)
	float elasticity;
	float circle_elasticity;
	float fric;

	float m_mass;
	float m_radius;
	bool stationary;
	bool collided;	
	bool debug_possible_col;
	float inertia;
	float orientation; //orientation of circle

	Vector2f extern_force;//is actually friction, whoops
	Vector2f m_force;
	Vector2f airjet_force;

	Vector2f angular_velocity;
	Vector2f angular_accelration;

	float rest_ticker; //counter to determine when the ball have rested
	//bool line_collided;
	Vector2f impact_point;
	Vector2f m_pos;

	Vector2f center_of_mass;//center of mass is just pos of circle(center of circle)
	Vector2f pt_on_circle;

	Vector2f m_newPos;
	Vector2f m_velocity;
	Vector2f m_newVelocity;
	ManifoldPoint earliest_contact;

	int m_objectID;
	static int countID;
};

