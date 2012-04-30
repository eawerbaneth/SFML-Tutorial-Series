#ifndef QUATERNION_H
#define QUATERNION_H

#include "matrix.h"


class xquat{
public:
	xquat();
	xquat(xvector a, float theta);
	xquat(vector <float> new_quat);
	xquat(xmatrix mat);
	void set_quat(vector <float> new_quat);
	xmatrix convert_to_matrix();
	//multiply two quaternions
	inline xquat operator*(xquat other);
	//multiply a quaternion by a scalar
	inline xquat operator*(float scalar);
	//dot product
	inline float operator%(xquat other);
	inline xquat operator+(xquat other);
	vector <float> get_quat(){return my_quat;}
	void reset();

private:
	vector <float> my_quat;
	void normalize();

};

void xquat::reset(){
	my_quat.clear();
	my_quat.push_back(1);
	for(int i=0; i<3; i++)
		my_quat.push_back(0);
}

void xquat::normalize(){
	//normalize the quaternion
	float magnitude = 0;
	for(int i=0; i<4; i++)
		magnitude += pow(my_quat[i], 2);
	magnitude = sqrt(magnitude);
	for(int i=0; i<4; i++)
		my_quat[i] /= magnitude;
}

//add two quats togethe
inline xquat xquat::operator+(xquat other){
	vector <float> new_quat;
	for(int i=0; i<4; i++)
		new_quat.push_back(my_quat[i]+other.get_quat()[i]);

	return xquat(new_quat);

}

//multiply quat by a scalar
inline xquat xquat::operator*(float scalar){
	for(int i=0; i<4; i++)
		my_quat[i] = my_quat[i]*scalar;
	return *this;
}

//dot product operator
inline float xquat::operator%(xquat other){
	vector <float> other_quat = other.get_quat();
	float product = 0;
	for(int i=0; i<4; i++)
		product += other_quat[i]*my_quat[i];

	return product;
}

//regular multiplication
inline xquat xquat::operator*(xquat other){
	vector <float> new_quat;
	vector<float> other_quat = other.get_quat();
	//my q0 row comes first
	float val = my_quat[0]*other_quat[0];
	for(int i=1; i<4; i++)
		val -= my_quat[i]*other_quat[i];
	new_quat.push_back(val);
	//ugly rows (i, j, k)
	new_quat.push_back(my_quat[1]*other_quat[0] + my_quat[0]*other_quat[1] + my_quat[2]*other_quat[3] - my_quat[3]*other_quat[2]);
	new_quat.push_back(my_quat[2]*other_quat[0] + my_quat[0]*other_quat[2] + my_quat[3]*other_quat[1] - my_quat[1]*other_quat[3]);
	new_quat.push_back(my_quat[3]*other_quat[0] + my_quat[0]*other_quat[3] + my_quat[1]*other_quat[2] - my_quat[2]*other_quat[1]);

	return xquat(new_quat);
}

xquat::xquat(){
	my_quat.push_back(1);
	for(int i=0; i<3; i++)
		my_quat.push_back(0);
}

xquat::xquat(vector<float> new_quat){
	my_quat = new_quat;
	normalize();
}

xquat::xquat(xmatrix mat){
	float q[4] = {0, 0, 0, 0};
	int next[3] = {1, 2, 0};
	for(int i=0; i<4; i++)
		my_quat.push_back(0);
	vector <vector <float>> inner_matrix = mat.get_inner_matrix();

	float trace = inner_matrix[0][0] + inner_matrix[1][1] + inner_matrix[2][2];
	if(trace > 0){
		float s = sqrt(trace + 1);
		my_quat[0] = s/2;
		s = .5/s;
		my_quat[1] = (inner_matrix[2][1] - inner_matrix[1][2])*s;
		my_quat[2] = (inner_matrix[0][2] - inner_matrix[2][0])*s;
		my_quat[3] = (inner_matrix[1][0] - inner_matrix[0][1])*s;
	}
	else{
		int i = 0;
		if(inner_matrix[1][1] > inner_matrix[0][0])
			i = 1;
		if(inner_matrix[2][2] > inner_matrix[i][i])
			i = 2;
		int j = next[i];
		int k = next[j];
		float s = sqrt((inner_matrix[i][i] - (inner_matrix[j][j] + inner_matrix[k][k])) + 1);
		q[i] = s*.5;
		if(s != 0)
			s = .5/s;
		my_quat[0] = (inner_matrix[j][k] - inner_matrix[k][j])*s;
		q[j] = (inner_matrix[i][j] + inner_matrix[j][i])*s;
		q[k] = (inner_matrix[i][k] + inner_matrix[k][i])*s;
		my_quat[1] = q[0];
		my_quat[2] = q[1];
		my_quat[3] = q[2];
	}
}

xmatrix xquat::convert_to_matrix(){
	vector <vector<float>> mat_skeleton;

	for(int i=0; i<3; i++){
		vector<float> new_row;
		for(int k=0; k<3; k++){
			//forward diagonal
			if(i==k){
				if(i==0)
					new_row.push_back(1-2*pow(my_quat[2], 2)-2*pow(my_quat[3], 2));
				if(i==1)
					new_row.push_back(1-2*pow(my_quat[1], 2)-2*pow(my_quat[3], 2));
				if(i==2)
					new_row.push_back(1-2*pow(my_quat[1], 2)-2*pow(my_quat[2], 2));
				//tried to do this algorithmically, but the stupid middle row messed it up
				//new_row.push_back(1 - 2*pow(my_quat[(i-2+3)%3 +1], 2) - 2*pow(my_quat[(i-1+3)%3+1], 2));
			}
			else if(i+k==1)
				new_row.push_back(2*my_quat[1]*my_quat[2] - (-1*i%2)*2*my_quat[0]*my_quat[3]);
			//can't do an even-odd mod-2 trick on this diagonal
			else if(i+k==2){
				if(k==0)
					new_row.push_back(2*my_quat[1]*my_quat[3] - 2*my_quat[0]*my_quat[2]);
				else
					new_row.push_back(2*my_quat[1]*my_quat[3] - 2*my_quat[0]*my_quat[2]);
			}
			else
				new_row.push_back(2*my_quat[2]*my_quat[3] + ((i-1)%2*-1)*2*my_quat[0]*my_quat[1]);
		}
		mat_skeleton.push_back(new_row);
	}

	xmatrix retmat(mat_skeleton, "rotation");

	return retmat;

}

xquat::xquat(xvector a, float theta){
	//q0 is going to be my q[0]
	my_quat.push_back(cos(theta/2));
	for(int i=0; i<3; i++)
		my_quat.push_back(a.get_coord(i)*sin(theta/2));

	normalize();

}

void xquat::set_quat(vector <float> new_quat){
	my_quat = new_quat;
}

//takes two quats and a beta value between 0 and 1, returns an angle
xquat slerp(xquat p, xquat q, float beta){
	//dot product
	float theta = acos(p%q);
	float wp = sin((1-beta)*theta)/sin(theta);
	float wq = sin(beta*theta)/sin(theta);
	
	return p*wp + q*wq;

}


#endif