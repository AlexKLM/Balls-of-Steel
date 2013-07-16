#include "Circle.h"
#include <Windows.h>
#include <gl\gl.h>  
#define _USE_MATH_DEFINES
#include <math.h>

int Circle::countID = 0;
//http://www.real-world-physics-problems.com/rolling.html
//http://physics.info/rolling/summary.shtml //rotational
//http://www.real-world-physics-problems.com/rolling-without-slipping.html


Circle::Circle(void) :  m_radius(0.18)
{
	fric = 0.02;
	elasticity = 0.5;
	circle_elasticity = 0.9;
	fixed =  false;
	//srand (time(NULL));
	airjet_force = Vector2f(0,0);
	m_mass = rand() % 10 + 3;
	rest_ticker = 0;
	stationary = false;
	collided = false;
	//line_collided = false;
	impact_point = Vector2f(0,0);
	m_objectID = countID;
	stationary = false;
	inertia = (m_mass*(m_radius*m_radius))/2;
	m_newPos.Set(0,0);
	m_newVelocity.Set(0,0);
	//float Tm = Vector2f(1.962,0).length();
	//float a = Tm/inertia;
	orientation = 0;
	//angular_velocity = 0;
	//angular_accelration = 0;
	angular_velocity = Vector2f(0,0);
	angular_accelration = Vector2f(0,0);
	//a * 180 / M_PI;
	++countID;
}

Circle::Circle(Vector2f Pos, float _radius, float _mass)
{
	fric = 0.02;
	elasticity = 0.5;
	circle_elasticity = 0.9;
	airjet_force = Vector2f(0,0);
	m_pos = Pos;
	m_mass = _mass;
	m_radius = _radius;
	rest_ticker = 0;
	stationary = false;
	collided = false;
	m_newPos.Set(0,0);
	m_newVelocity.Set(0,0);
	//line_collided = false;
	impact_point = Vector2f(0,0);
	m_objectID = countID;
	stationary = false;
	inertia = (m_mass*(m_radius*m_radius))/2;
	//float Tm = Vector2f(1.962,0).length();
	//float a = Tm/inertia;
	orientation = 0;
	//angular_velocity = 0;
	//angular_accelration = 0;
	angular_velocity = Vector2f(0,0);
	angular_accelration = Vector2f(0,0);
	//a * 180 / M_PI;
	++countID;
}

Circle::~Circle(void)
{
}

void Circle::move(float dt)
{
	//m_pos = m_newPos;
	if(!fixed)
	{
		m_newPos = m_pos + ((m_velocity + m_newVelocity)/2* dt);
	}
}

void Circle::moveRK(float dt)
{
	m_newPos = m_pos + (m_newVelocity*dt);
}

void Circle::SetAirforce(Vector2f force)
{
	airjet_force = force;
	stationary = false; // force next intergration to happen, ball will have to move anyway
}

void Circle::SetPos(float x, float y)
{
	m_pos.Set(x, y);
	center_of_mass.Set(x,y);
	pt_on_circle.Set(x+m_radius,y+m_radius);
	if(m_pos.GetY() < -20.0f)
	{
		m_pos.Set(m_pos.GetX(), -20.0f);
	}
}

void Circle::add_force(float x, float y)
{
	extern_force.Set(x,y);
}

void Circle::SetVel(float x, float y)
{
	m_velocity.Set(x, y);
}

void Circle::SetFric(float input)
{
	fric = input;
}

void Circle::SetElas(float input)
{
	elasticity = input;
}

void Circle::SetCirElas(float input)
{
	circle_elasticity = input;
}

void Circle::SetNewVel(Vector2f vel)
{
	m_newVelocity = vel;
}

void Circle::SetMass(float mass)
{
	m_mass = mass;
}

bool Circle::is_stationary()
{
	return stationary;
}

void Circle::CalculatePhysicsImE(float dt)
{
	Vector2f force(0.0f, -9.81f * m_mass);
	force =  force + (airjet_force);
	m_force = force/m_mass;
	Vector2f drag(0.1f,0.1f); //Cf
	drag = drag /m_mass;
	drag = (drag*m_velocity)*-1;
	Vector2f accel = (force / m_mass)+drag;
	m_newVelocity = m_velocity + (accel * dt);
		
	angular_accelration =  m_newVelocity;//cheating whoops

	Vector2f normAA = angular_accelration;
	normAA.normalise();
	float dir = Vector2f(0,1).dot(normAA);
	if(dir < 0 )
	{
		angular_velocity = angular_velocity - (angular_accelration * dt);
		
		orientation -= angular_accelration.GetX() * dt*2;
			
	}
	else
	{
		angular_velocity = angular_velocity + (angular_accelration * dt);
		orientation += angular_accelration.GetX() * dt*2;
	}

	move(dt);
	//Vector2f f = calculate_friction(1,dt);

	//m_newPos = m_pos + ((m_velocity + m_newVelocity)/2 * dt);

	// ********************************
	// Handle ground collision
	// This should be handled properly!
	// ********************************
	/*if(m_newPos.GetY() < -5.0f)
	{
		m_newPos.Set(m_newPos.GetX(), 4.0f);
		m_newVelocity.Set(m_newVelocity.GetX(), 0.0f);
	}*/
	airjet_force.Set(0,0);
}

void Circle::CalculatePhysicsRK(float dt)
{
	Vector2f force(0.0f, -9.81f * m_mass);
	force =  force + (airjet_force*m_mass);
	m_force = force/m_mass;

	Vector2f drag(0.1f,0.1f); //Cf
	drag = drag /m_mass;
	drag = (drag*m_velocity)*-1;
	Vector2f accel = (force / m_mass)+drag;
	Vector2f k1,k2,k3,k4;

	k1 =  (accel * 0);
	k2 =((k1/2)+(accel * (dt*0.5)));
	k3 = ((k2/2)+(accel*(dt*0.5)));
	k4 = (k3+(accel*dt));

	m_newVelocity =m_velocity+ ((k1+(2*k2)+(2*k3)+k4)*1/6)*dt  ;

	Vector2f normAA = angular_accelration;
	normAA.normalise();
	float dir = Vector2f(0,1).dot(normAA);
	if(dir < 0 )
	{
		angular_velocity = angular_velocity - (angular_accelration * dt);

		orientation -= angular_accelration.GetX() * dt*2;

	}
	else
	{
		angular_velocity = angular_velocity + (angular_accelration * dt);
		orientation += angular_accelration.GetX() * dt*2;
	}

	moveRK(dt);

	
	airjet_force.Set(0,0);
}

void Circle::containment()
{
	if(m_newPos.GetY() < -5.0f+m_radius)
	{
		m_newPos.Set(m_newPos.GetX(), -5.0f+m_radius);
		m_newVelocity.Set(m_newVelocity.GetX(), 0.0f);
	}
	else if(m_newPos.GetY() > 5-m_radius)
	{
		m_newPos.Set(m_newPos.GetX(), 5.0f-m_radius);
		m_newVelocity.Set(m_newVelocity.GetX(), 0.0f);
	}
	if(m_newPos.GetX() < -5.0f+m_radius)
	{
		m_newPos.Set(-5.0f+m_radius, m_newPos.GetY());
		m_newVelocity.Set(0.0f, m_newVelocity.GetY());
	}
	else if(m_newPos.GetX() > 5-m_radius)
	{
		m_newPos.Set(5.0f-m_radius, m_newPos.GetY());
		m_newVelocity.Set(0.0f, m_newVelocity.GetY());
	}
}

void Circle::CalculatePhysicsImE(Vector2f V, float dt)
{
	if(fixed)
	{
		return;
	}
	Vector2f force(0.0f, -9.81f * m_mass);
	force =  force + extern_force;
	m_force = force/m_mass;
	Vector2f drag(0.1f,0.1f); //Cf
	drag = drag /m_mass;
	drag = (drag*m_velocity)*-1;
	Vector2f accel = (force / m_mass)+drag;
	if(extern_force.length() > m_newVelocity.length())
	{
		m_velocity.Set(0,0);
		m_newVelocity.Set(0,0);
		return;
	}
	m_newVelocity = V + (accel * dt);
	
	angular_accelration = m_newVelocity;//cheating whoops

Vector2f normAA = angular_accelration;
	normAA.normalise();
	float dir = Vector2f(0,1).dot(normAA);
	if(dir < 0 )
	{
		angular_velocity = angular_velocity - (angular_accelration * dt);
		
		orientation -= angular_accelration.GetX() * dt*2;
			
	}
	else
	{
		angular_velocity = angular_velocity + (angular_accelration * dt);
		orientation += angular_accelration.GetX() * dt*2;
	}
	//Vector2f f = calculate_friction(1,dt);
	move(dt);
	//m_newPos = m_pos + ((m_velocity + m_newVelocity)/2 * dt);
	extern_force.Set(0,0);
	// ********************************
	// Handle ground collision
	// This should be handled properly!
	// ********************************

}

void Circle::CalculatePhysicsRK(Vector2f V, float dt)
{
	if(fixed)
	{
		return;
	}

	Vector2f force(0.0f, -9.81f * m_mass);
	force =  force + extern_force;
	m_force = force/m_mass;

	Vector2f drag(0.1f,0.1f); //Cf
	drag = drag /m_mass;
	drag = (drag*m_velocity)*-1;
	Vector2f accel = (force / m_mass)+drag;
	Vector2f k1,k2,k3,k4;

	k1 = V + (accel * 0);
	k2 = V + (k1/2+(accel * (dt*0.5)));
	k3 = V + (k2/2+(accel*(dt*0.5)));
	k4 = V + (k3/2+(accel*dt));

	m_newVelocity =V+ ((k1+(2*k2)+(2*k3)+k4)*1/6)*dt ;


	Vector2f normAA = angular_accelration;
	normAA.normalise();
	float dir = Vector2f(0,1).dot(normAA);
	if(dir < 0 )
	{
		angular_velocity = angular_velocity - (angular_accelration * dt);

		orientation -= angular_accelration.GetX() * dt*2;

	}
	else
	{
		angular_velocity = angular_velocity + (angular_accelration * dt);
		orientation += angular_accelration.GetX() * dt*2;
	}

	moveRK(dt);
	extern_force.Set(0,0);
	if(m_newPos.GetY() < -5.0f)
	{
		m_newPos.Set(m_newPos.GetX(), 4.0f);
		m_newVelocity.Set(m_newVelocity.GetX(), 0.0f);
	}
}

void Circle::set_stationary(bool input)
{
	stationary = input;
}

//----------------------------------------------------------------
//Collision Detection
//----------------------------------------------------------------

bool Circle::CollisionWithCircle(Circle* circle, float dt)
{

	Vector2f relative_velocity = m_velocity - circle->GetVel();

	Vector2f position = m_pos - circle->GetPos();

	//closest time = -b-(+)sqrt((b*b)-4*a*c/2a
	float a = relative_velocity.dot(relative_velocity);
	float b = 2 * position.dot(relative_velocity);
	float c = (position.dot(position)) -(m_radius + circle->GetRadius())*(m_radius + circle->GetRadius());
	
	float d = b*b-4*a*c;

	float time;
	if(d<0)
	{
		//negative time not possible;
		//collided = false;
		return false;
	}
	else
	{
		//positive or zero, impossible to detect against zero so assume its just positive
		float t0 = (-b + sqrt(d))/(2*a);
		float t1 = (-b - sqrt(d))/(2*a);
		time = min(t0,t1);
		if(time > 0)
		{
			if(time < dt)
			{
				ManifoldPoint mPoint;
				impact_point = m_pos + ((m_velocity + m_newVelocity)/2 * time);
				Vector2f C2_impact_point = circle->GetPos() + ((circle->GetVel() + circle->GetNewVel())/2 * time);
				Vector2f contact_line = impact_point - C2_impact_point;
				//m_newPos = impact_point;
				//Vector2f circle2_newpos = circle->GetPos() + (time*circle->GetNewVel());
				//circle->SetNewPos(circle2_newpos.GetX(),circle2_newpos.GetY());
				Vector2f impact_normal = contact_line.normalise();
				mPoint.contactID1 = this;
				mPoint.contactID2 = circle;
				mPoint.contactNormal = impact_normal;
				mPoint.contactPoint = impact_point;
				mPoint.contact_time = time;
				mPoint.type = with_circle;
				mPoint.responded = false;
				//contactManifold->Add(mPoint);
				//contactManifold->Add_debug(impact_point);
				//collision happen within the timeframe, add to manifold
				collided = true;
				if(earliest_contact.responded || !earliest_contact.responded&& earliest_contact.contact_time > time)
				{
					earliest_contact = mPoint;
				}
				return true;
			}
		}
	}
	return false;

}

bool Circle::CollisionWithLineEnd(Vector2f P, float dt)
{
	Vector2f relative_velocity = m_velocity;
	Vector2f position = m_pos - P;

	//closest time = -b-(+)sqrt((b*b)-4*a*c/2a
	float a = relative_velocity.dot(relative_velocity);
	float b = 2 * position.dot(relative_velocity);
	float c = (position.dot(position)) -(m_radius)*(m_radius);
	
	float d = b*b-4*a*c;

	float time;
	if(d<0)
	{
		//negative time not possible;
		//collided = false;
		return false;
	}
	else
	{
		float t0 = (-b + sqrt(d))/(2*a);
		float t1 = (-b - sqrt(d))/(2*a);
		time = min(t0,t1);
		if(time > 0)
		{
			if(time < dt)
			{
				ManifoldPoint mPoint;
				impact_point = m_pos + ((m_velocity + m_newVelocity)/2 * time);

				mPoint.contactID1 = this;
				mPoint.contactPoint = impact_point;
				Vector2f contact_line = impact_point - P;
				mPoint.contactNormal = contact_line.normalise();
				mPoint.contact_time = time;
				mPoint.type = with_line_end;
				mPoint.responded = false;

				collided = true;
				if(earliest_contact.responded || !earliest_contact.responded&& earliest_contact.contact_time > time)
				{
					earliest_contact = mPoint;
				}
				return true;
			}
		}
	}
	return false;
}

bool Circle::CollisionWithLine(line* _line, float dt)
{
	bool line_collided = false;
	//based on the function in "Real time collision dectetion"

	float d = _line->GetNormal().dot(_line->GetP2());
	float dist = (_line->GetNormal().dot(m_pos)) - d;
	Vector2f point_on_line;
	float t = 0,r = 0;
	float denom = _line->GetNormal().dot(m_newVelocity);
	if(CollisionWithLineEnd(_line->GetP1(),dt) || CollisionWithLineEnd(_line->GetP2(),dt))
	{
		return true;
	}
	if(dist > 0.0f)
	{
		//sphere "infront" of plane
		r = m_radius;
	}
	else
	{
		//sphere "behind" of plane
		r = -m_radius;
	}
	t = (r - dist)/denom; //calculate time of collision
	if(t > 0)
	{
		if(t<dt)
		{

			impact_point = m_pos + ((m_newVelocity) * t);
			point_on_line = m_pos + ((m_newVelocity) * t).subtract((r * _line->GetNormal())); 
			line_collided = _line->is_in_line(point_on_line.GetX(),point_on_line.GetY());
			if(line_collided)
			{
				collided = true;
				ManifoldPoint maniPoint;
				maniPoint.contactID1 = this;
				maniPoint.contactPoint = impact_point;
				maniPoint.contact_time = t;
				maniPoint.contactNormal = _line->GetNormal();
				maniPoint.type = with_line;
				maniPoint.responded = false;
				if(earliest_contact.responded || !earliest_contact.responded && earliest_contact.contact_time > t)
				{
					earliest_contact = maniPoint;
				}
				//contactManifold->Add_debug(impact_point);
				//contactManifold->Add(maniPoint);
			}
			return true;

		}
	}
	return false;
	//}


}

bool Circle::will_collide()
{
	if(earliest_contact.responded)
	{
		return false;
	}
	else
	{
		return true;
	}
}

float Circle::get_contact_point()
{
	return earliest_contact.contact_time;
}

void Circle::ResetPos()
{
	m_newPos = m_pos;
}

void Circle::Update(float dt)
{
	containment();
	m_velocity = m_newVelocity;

	if(!fixed)
	{
		if(m_velocity.length() < 0.025)
		{
			if(rest_ticker > 2) //ball have spent 10 frame not moving alot
			{
				angular_velocity.Set(0,0);
				stationary = true;
			}
			else
			{
				rest_ticker+=dt;
			}
		}
		else
		{
			rest_ticker = 0;
			stationary = false;
		}
	}
	m_pos = m_newPos;
	pt_on_circle.Set(m_pos.GetX(), m_pos.GetY() + m_radius);
	extern_force.Set(0,0);
}

void Circle::Calculate_friction(Vector2f &normal, Vector2f &v, float e,bool add_to_net)
{
	Vector2f MG = (Vector2f(0,9.81)*m_mass)*normal;
	Vector2f fric_dir = v;
	fric_dir = (fric_dir.normalise()*-1); //make sure its pointing the other way
	float fric_mag = (fric*MG).length();
	Vector2f fric_force = fric_mag * fric_dir;
	if(add_to_net)
	{
		extern_force = fric_force;
	}
	Vector2f Torque = (m_radius * fric_force)*-1; //cheating, always use the point on the opposite side of the circle
	
	//angular_accelration = Torque/inertia;
	

}

void Circle::set_fixed(bool input)
{
	fixed = input;
}

bool Circle::is_fixed()
{
	return fixed;
}


//--------------------------------------
// COLLISION RESPONSES
//--------------------------------------

void Circle::CollisionResponseWithLine(ManifoldPoint &point,float dt)
{
	//Vr = Vi-2(Vi.N)N
	//Vr = Vi - 2(VN)
	Vector2f Vi = m_newVelocity;
	float Vi_VN_a = Vi.dot(point.contactNormal);
	Vector2f VN = (Vi.dot(point.contactNormal))*point.contactNormal;
	Vector2f new_new_velocity = Vi - ((1+elasticity)*(VN));
	

	float remaning_time = dt - point.contact_time;

	m_velocity = Vector2f(0,0);
	m_pos = point.contactPoint;
	


	Calculate_friction(point.contactNormal,new_new_velocity,0.05,true);
	CalculatePhysicsImE(new_new_velocity,remaning_time);
	point.remaining_time = remaning_time;
	collided = false;
	point.responded = true;
	earliest_contact.responded = true;
}

void Circle::CollisionResponseWithCircle(ManifoldPoint &point,float dt)
{
	Vector2f V1;
	Vector2f V2;
	float M2 = point.contactID2->GetMass();
	Vector2f U2 = point.contactID2->GetNewVel();
	Vector2f Normal = point.contactNormal;
	float r_time = dt - point.contact_time;
	Vector2f C1_contactP = m_pos + ((m_velocity + m_newVelocity)/2 * point.contact_time);
	Vector2f C2_contactP = point.contactID2->GetPos() +((point.contactID2->GetVel() + point.contactID2->GetNewVel())/2 * point.contact_time);
	float e = circle_elasticity;
	V1 = ((m_mass - e*M2)*(m_newVelocity.dot(Normal)*Normal)+((M2+e*M2)*(U2.dot(Normal)*Normal)))/(m_mass+M2);

	V2 = ((m_mass + e*m_mass)*(m_newVelocity.dot(Normal)*Normal)+((M2-e*m_mass)*(U2.dot(Normal)*Normal)))/(m_mass+M2);

	V1 = m_velocity - ((m_velocity.dot(Normal))*Normal) + V1;

	V2 = U2 - ((U2.dot(Normal))*Normal)+V2;
	SetVel(0,0);
	m_newVelocity = V1;
	m_pos = C1_contactP;

	Calculate_friction(point.contactNormal,V1,0.05,false);
	point.contactID2->Calculate_friction(point.contactNormal,V2,0.05,false);


	Vector2f c2_newPos = C2_contactP;
	point.contactID2->SetPos(c2_newPos.GetX(),c2_newPos.GetY());
	point.contactID2->SetVel(0,0);
	//point.contactID2->SetNewVel(V2);
	CalculatePhysicsImE(V1,r_time);
	point.contactID2->CalculatePhysicsImE(V2,r_time);



	//move(r_time);
	//point.contactID2->move(r_time);
	collided = false;
	point.responded = true;
	earliest_contact.responded = true;
}

void Circle::CollisionResponseWithLineEnd(ManifoldPoint &point, float dt)
{
	float e = elasticity;
	Vector2f U2 = m_newVelocity*-1;
	//Vector2f VN = ((m_newVelocity.dot(point.contactNormal))*point.contactNormal).normalise();
	//m_newVelocity = (-e*(m_newVelocity)*VN);
	Vector2f V1;
	Vector2f Normal = point.contactNormal;
	float M2 = 1;
	V1 = ((m_mass - e*M2)*(m_newVelocity.dot(Normal)*Normal)+((M2+e*M2)*(U2.dot(Normal)*Normal)))/(m_mass+M2);
	m_newVelocity = m_velocity - ((m_velocity.dot(Normal))*Normal) + V1;
	m_newPos = point.contactPoint;
	float r_time = dt - point.contact_time;
	SetVel(0,0);
	move(r_time);
	point.responded = true;
	earliest_contact.responded = true;
	collided = false;
}

float Circle::GetMass() const
{
	return m_mass;
}

Vector2f Circle::GetPos() const
{
	return m_pos;
}

Vector2f Circle::GetNewPos() const
{
	return m_newPos;
}

Vector2f Circle::GetVel() const
{
	return m_velocity;
}

Vector2f Circle::GetNewVel() const
{
	return m_newVelocity;
}

float Circle::GetRadius() const
{
	return m_radius;
}

void Circle::set_pos_col(bool input)
{
	debug_possible_col = input;
}

float Circle::GetAngle() const
{
	return orientation;
}

int Circle::getID()
{
	return m_objectID;
}

void Circle::SetNewPos(float x, float y)
{
	m_newPos.Set(x, y);
}

void Circle::add_to_manifold(ContactManifold *contactManifold)
{
	if(stationary && earliest_contact.type == with_line)
	{
		earliest_contact.responded = true;
		collided = false;
		return;
	}
	else if(earliest_contact.responded)
	{
		stationary = false;
		collided = false;
	}
	else if(!earliest_contact.responded)
	{
		contactManifold->Add(earliest_contact);
	}
}

bool Circle::have_collided()
{
	return collided;
}

AABB Circle::get_Bbox_static()
{
	AABB box;
	box.min = Vector2f(m_pos.GetX()- m_radius,m_pos.GetY() - m_radius);
	box.max = Vector2f(m_pos.GetX() + m_radius, m_pos.GetY()+m_radius);
	return box;
}