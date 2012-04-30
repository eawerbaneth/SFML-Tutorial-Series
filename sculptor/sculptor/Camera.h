#include <SFML/OpenGL.hpp>
#include <math.h>
#include <iostream>
#include <Windows.h>

#define SQR(x) (x*x)


struct vec3{
	GLfloat x, y, z;
};

vec3 TemplateVec(float new_x, float new_y, float new_z){
	vec3 temp;
	temp.x = new_x;
	temp.y = new_y;
	temp.z = new_z;
	return temp;
}

struct vec2{
	GLfloat x, y;
};

class XCamera{
public:
	XCamera();			//init vals to 0, 0, 0, target to 0, 0, -1
	void Render(void); //execs some glRotates and a glTranslate command
						//note: you should call glLoadIdentity before using Render

	void Move(vec3 Direction);
	void RotateX(GLfloat Angle);
	void RotateY (GLfloat Angle);
	void RotateZ(GLfloat Angle);

	void MoveForward(GLfloat Distance);
	void MoveUpward(GLfloat Distance);
	void StrafeRight(GLfloat Distance);

//private:
	vec3 ViewDir;
	vec3 RightVector;
	vec3 UpVector;
	vec3 Position;

	GLfloat RotatedX, RotatedY, RotatedZ;

};

vec3 F3dVector( GLfloat x, GLfloat y, GLfloat z){
	vec3 temp;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

GLfloat GetVecLength(vec3 *v){
	return (GLfloat)(sqrt(SQR(v->x) + SQR(v->y) + SQR(v->z)));
}

vec3 NormalizeVec(vec3 v){
	vec3 res;
	float l = GetVecLength(&v);
	if(l == 0.0f)
		res.x = res.y = res.z = 0.0f;
	else{
		res.x = v.x/l;
		res.y = v.y/l;
		res.z = v.z/l;
	}
	return res;

}


bool operator==(vec3 v, vec3 u){
	if(v.x == u.x && v.y == u.y && v.z == u.z)
		return true;
	return false;
}

vec3 operator+(vec3 v, vec3 u){
	vec3 res;
	res.x = v.x + u.x;
	res.y = v.y + u.y;
	res.z = v.z + u.z;
	return res;
}

vec3 operator-(vec3 v, vec3 u){
	vec3 res;
	res.x = v.x - u.x;
	res.y = v.y - u.y;
	res.z = v.z - u.z;
	return res;
}

float operator*(vec3 v, vec3 u){//dot product
	//vec3 res;
	return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

vec3 operator*(vec3 v, float r){
	vec3 res;
	res.x = v.x*r;
	res.y = v.y*r;
	res.z = v.z*r;

	return res;
}

vec3 CrossProduct(vec3 *u, vec3* v){
	vec3 resVector;
	resVector.x = u->y*v->z - u->z*v->y;
	resVector.y = u->z*v->x - u->x*v->z;
	resVector.z = u->x*v->y - u->y*v->x;

	return resVector;
}

const float PIdiv180 = 0.0174532925;

//CAMERA IMPLEMENTATIN
XCamera::XCamera(){
	//init with standard OpenGL values
	Position.x = Position.y = Position.z = 0.0f;
	ViewDir.x = ViewDir.y = 0.0f; ViewDir.z = -1.0f;
	RightVector.x = 1.0f; RightVector.y = RightVector.z = 0.0f;
	UpVector.x = UpVector.z = 0.0f; UpVector.y = 1.0f;

	RotatedX = RotatedY = RotatedZ = 0.0f;

}

void XCamera::Move(vec3 Direction){
	Position = Position + Direction;
}

void XCamera::RotateX(GLfloat Angle){
	RotatedX += Angle;
	
	//Rotate viewdir around the right vector:
	ViewDir = NormalizeVec(ViewDir*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = CrossProduct(&ViewDir, &RightVector)*-1;
}

void XCamera::RotateY(GLfloat Angle){
	RotatedY += Angle;
	
	//Rotate viewdir around the up vector:
	ViewDir = NormalizeVec(ViewDir*cos(Angle*PIdiv180)
								- RightVector*sin(Angle*PIdiv180));

	//now compute the new RightVector (by cross product)
	RightVector = CrossProduct(&ViewDir, &UpVector);
}

void XCamera::RotateZ(GLfloat Angle){
	RotatedZ += Angle;
	
	//Rotate viewdir around the right vector:
	RightVector = NormalizeVec(RightVector*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = CrossProduct(&ViewDir, &RightVector)*-1;
}

void XCamera::Render(void){
	//the point at which the camera looks:
	vec3 ViewPoint = Position+ViewDir;

	//glMatrixMode(GL_PROJECTION);
	
	//glLoadIdentity();

	//as we know the up vector, we can easly use gluLookAt:
	gluLookAt(Position.x, Position.y, Position.z,
		ViewPoint.x, ViewPoint.y, ViewPoint.z,
		UpVector.x, UpVector.y, UpVector.z);

	//glMatrixMode(GL_MODELVIEW);

}

void XCamera::MoveForward(GLfloat Distance){
	Position = Position + (ViewDir*-Distance);
}

void XCamera::StrafeRight(GLfloat Distance){
	Position = Position + (RightVector*Distance);
}

void XCamera::MoveUpward(GLfloat Distance){
	Position = Position + (UpVector*Distance);
}