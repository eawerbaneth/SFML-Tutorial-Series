#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include "auxiliary.h"
using namespace std;

#pragma warning(disable:4244)
#pragma warning(disable:4018)

//matrix class
class xmatrix{
public:
	xmatrix(){}
	xmatrix(vector <vector <float>> new_matrix, string what_type);
	xmatrix(string type, float val);
	xmatrix(xvector trans);
	vector <float> get_row(int row_num){return inner_matrix[row_num];}
	int size(){return inner_matrix.size();}
	//matrix multiplication operator v' = M*v
	inline xpoint operator*(xpoint point){
		//create temps
		vector <float> new_coords;

		//iterate through matrix rows
		for(int m=0; m<inner_matrix.size()-1; m++){
			float new_coord = 0;
			//iterate through point col and perform multiplication
			for(int p=0; p<point.size(); p++)
				new_coord += inner_matrix[m][p]*point.get_ele(p);
			new_coords.push_back(new_coord);
		}

		return xpoint(new_coords);
	}
	inline xmatrix operator*(float scalar){
		vector <vector <float>> new_mat;
		for(int i=0; i<3; i++){
			vector <float> new_row;
			for(int k=0; k<3; k++)
				new_row.push_back(inner_matrix[i][k]*scalar);
			new_mat.push_back(new_row);
		}
		return xmatrix(new_mat, xtype);

	}
	//matrix multiplication(by vector)
	inline xvector operator*(xvector vec);

	vector <vector <float>> get_inner_matrix(){return inner_matrix;}
	void print();

private:
	vector <vector <float>> inner_matrix;
	//keeps track of which type of matrix it is
	string xtype;

};

inline xvector xmatrix::operator*(xvector vec){
	vector <float> new_vec;

	for(int m=0; m<inner_matrix.size()-1; m++){
		float new_ele = 0;
		for(int v=0; v<vec.size(); v++)
			new_ele += inner_matrix[m][v]*vec.get_coord(v);
		new_vec.push_back(new_ele);
	}

	return xvector(new_vec);
}
//constructor for translateXYZ that takes a vector
xmatrix::xmatrix(xvector trans){
	for(int i=0; i<3; i++){
		vector <float> new_row;
		for(int k=0; k<3; k++){
			if(i==k)
				new_row.push_back(1);
			else
				new_row.push_back(0);
		}
		new_row.push_back(trans.get_coord(i));
		inner_matrix.push_back(new_row);
	}

	//add the last row to the matrix
	vector <float> last_row;
	for(int i=0; i<3; i++)
		last_row.push_back(0);
	last_row.push_back(1);
	inner_matrix.push_back(last_row);

	xtype = "translation";

}

xmatrix::xmatrix(vector <vector <float>> new_matrix, string what_type){
	xtype = what_type;

	for(int i=0; i<3; i++){
		vector <float> new_row;
		for(int k=0; k<3; k++)
			new_row.push_back(new_matrix[i][k]);
		new_row.push_back(0);
		inner_matrix.push_back(new_row);
	}

	vector<float> last_row;
	for(int i=0; i<3; i++)
		last_row.push_back(0);
	last_row.push_back(1);
	inner_matrix.push_back(last_row);

}

//constructor for all other types
xmatrix::xmatrix(string type, float val){
	if(type == "translateX"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k < 3; k++){
				if(i==k)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}
		inner_matrix[0][3] = val;
	}
	else if(type == "translateY"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k < 3; k++){
				if(i==k)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}
		inner_matrix[1][3] = val;
	}
	else if(type == "translateZ"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k < 3; k++){
				if(i==k)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}
		inner_matrix[2][3] = val;
	}
	else if(type == "scale"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k < 3; k++){
				if(i==k)
					new_row.push_back(val);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}
	}
	else if(type == "rotateH"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k<3; k++){
				if(i==0 && k == 0)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}

		inner_matrix[1][1] = cos(val);
		inner_matrix[1][2] = -sin(val);
		inner_matrix[2][1] = sin(val);
		inner_matrix[2][2] = cos(val);


	}
	else if(type == "rotateP"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k<3; k++){
				if(i==1 && k == 1)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}

		inner_matrix[0][0] = cos(val);
		inner_matrix[0][2] = sin(val);
		inner_matrix[2][0] = -sin(val);
		inner_matrix[2][2] = cos(val);

	}
	else if(type == "rotateR"){
		for(int i=0; i < 3; i++){
			vector <float> new_row;
			for(int k=0; k<3; k++){
				if(i==2 && k == 2)
					new_row.push_back(1);
				else
					new_row.push_back(0);
			}
			new_row.push_back(0);
			inner_matrix.push_back(new_row);
		}

		inner_matrix[0][0] = cos(val);
		inner_matrix[0][1] = -sin(val);
		inner_matrix[1][0] = sin(val);
		inner_matrix[1][1] = cos(val);

	}
	else{
		cout << type << " is an invalid operation\n";

	}

	//add the last row to the matrix
	vector <float> last_row;
	for(int i=0; i<3; i++)
		last_row.push_back(0);
	last_row.push_back(1);
	inner_matrix.push_back(last_row);

	xtype = type;
}

/*
xquat xmatrix::convert_to_quat(){
	vector <float> new_quat;
	float q[4] = {0, 0, 0, 0};
	int next[3] = {1, 2, 0};
	for(int i=0; i<4; i++)
		new_quat.push_back(0);
	float trace = inner_matrix[0][0] + inner_matrix[1][1] + inner_matrix[2][2];
	if(trace > 0){
		float s = sqrt(trace + 1);
		new_quat[0] = s/2;
		s = .5/s;
		new_quat[1] = (inner_matrix[2][1] - inner_matrix[1][2])*s;
		new_quat[2] = (inner_matrix[0][2] - inner_matrix[2][0])*s;
		new_quat[3] = (inner_matrix[1][0] - inner_matrix[0][1])*s;
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
		new_quat[0] = (inner_matrix[j][k] - inner_matrix[k][j])*s;
		q[j] = (inner_matrix[i][j] + inner_matrix[j][i])*s;
		q[k] = (inner_matrix[i][k] + inner_matrix[k][i])*s;
		new_quat[1] = q[0];
		new_quat[2] = q[1];
		new_quat[3] = q[2];
	}

	return xquat(new_quat);
}*/

//prints the matrix
void xmatrix::print(){
	cout << xtype << " matrix:\n";
	for(int i=0; i<inner_matrix.size(); i++){
		cout << "\t";
		for(int k=0; k<inner_matrix.size(); k++){
			cout << inner_matrix[i][k] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}



#endif