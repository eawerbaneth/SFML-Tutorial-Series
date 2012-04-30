//points and vectors

#include <vector>
#include <iostream>
using namespace std;

#pragma warning(disable:4244)
#pragma warning(disable:4018)

//vector class
class xvector{
public:
	xvector(){}
	xvector(float x, float y, float z);
	xvector(vector <float> vec){my_vector = vec;}
	float get_coord(int index){return my_vector[index];}
	int size(){return my_vector.size();}
	//dot product
	inline float operator%(xvector other);
	//cross product
	inline xvector operator*(xvector other);

private:
	vector <float> my_vector;

};

//dot product
inline float xvector::operator%(xvector other){
	float product = 0;
	for(int i=0; i<my_vector.size(); i++)
		product += my_vector[i]*other.get_coord(i);
	return product;
}

//cross product
inline xvector xvector::operator*(xvector other){
	vector <float> new_vec;
	new_vec.push_back(my_vector[1]*other.get_coord(2) - my_vector[2]*other.get_coord(1));
	new_vec.push_back(my_vector[2]*other.get_coord(0) - my_vector[0]*other.get_coord(2));
	new_vec.push_back(my_vector[0]*other.get_coord(1) - my_vector[1]*other.get_coord(0));
	new_vec.push_back(0);
	return xvector(new_vec);
}

//constructor, automatically adds a 0 for the w-component
xvector::xvector(float x, float y, float z){
	my_vector.push_back(x);
	my_vector.push_back(y);
	my_vector.push_back(z);
	my_vector.push_back(0);
}

//point class
class xpoint{
public:
	xpoint(){}
	xpoint(float x, float y, float z);
	xpoint(vector <float> new_point);
	int size(){return my_point.size();}
	float get_ele(int index){return my_point[index];}
	inline xvector operator-(xpoint other);
	void print();


	vector<float> my_point;

};

//get a vector from other point to first point
inline xvector xpoint::operator-(xpoint other){
	vector <float> vec;
	for(int i=0; i<3; i++)
		vec.push_back(my_point[i]-other.get_ele(i));
	return xvector(vec);
}

//constructor, automatically adds a 1 for the w-component
xpoint::xpoint(float x, float y, float z){
	my_point.push_back(x);
	my_point.push_back(y);
	my_point.push_back(z);
	my_point.push_back(1);
}

//same deal here
xpoint::xpoint(vector <float> new_point){
	my_point = new_point;
	my_point.push_back(1);
}

//prints a point
void xpoint::print(){
	cout << "\t(" << my_point[0] << ", " << my_point[1] << ", " << my_point[2] << ")\n";
}
