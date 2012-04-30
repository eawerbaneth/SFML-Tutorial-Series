#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Camera.h"
#include "quaternion.h"
using namespace std;

#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4996)

const float PI = 3.14159265;

typedef struct{
    float XYZW[4];
    float RGBA[4];
	float NORMAL[3];
} Vertex;

bool operator==(Vertex a, Vertex b){
	for(int i=0; i < 4; i++){
		if(a.RGBA[i] != b.RGBA[i])
			return false;
		if(a.XYZW[i] != b.XYZW[i])
			return false;
	}

	return true;
}

struct Poly{
	vector <int> indices;
};

float get_dist(float a[], float b[]){
	return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2));
}

class OpenGLObject{
public:
	OpenGLObject::OpenGLObject();
	OpenGLObject::OpenGLObject(int v_offset);
	OpenGLObject::OpenGLObject(ifstream &input);
	vector <Vertex> aux_vertices;
	vector <GLuint> aux_indices;
	//utility functions
	bool GetVertex(GLdouble mouse_coords[]);
	bool GetEdge(GLdouble mouse_coords[]);
	bool GetPoly(GLdouble mouse_coords[]);
	
	bool GetObject(GLdouble mouse_coords[]);
	void reset_colors();
	void reset_selections();
	//translations
	void translate_vertex(float x, float y, float z);
	void translate_edge(float x, float y, float z);
	void translate_poly(float x, float y, float z);
	void translate_object(float x, float y, float z);
	//rotations
	void rotate_edge(float h, float p, float r);
	void rotate_poly(float h, float p, float r);
	void rotate_object(float h, float p, float r);
	void Finish_Rotate();
	//scaling
	void scale_edge(float s);
	void scale_poly(float s);
	void scale_object(float s);
	//split edge/poly
	void split_edge();
	void split_poly();

private:
	vector <Vertex> vertices;
	vector <Poly> polies;
	void prepare_indices();
	void prepare_vertices();
	Vertex null_vertex;
	//translation functions
	void trans_vert(vector <Vertex*> v, float x, float y, float z);
	void rot_vert(vector <Vertex*> v, float h, float p, float r);
	void scale_vert(vector <Vertex*> v, float s);
	void calculate_normals();

	//keep track of currently selected vertex, edge, and polygon
	Vertex* selected_vertex;
	vector <Vertex*> selected_edge;
	vector <Vertex*> selected_poly;

};

OpenGLObject::OpenGLObject(){
	//create a unit cube (in triangles)

	//create our indices
	/*
    0	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
    1	-1.0f,-1.0f, 1.0f,
    2	-1.0f, 1.0f, 1.0f, // triangle 1 : end
    3	1.0f, 1.0f,-1.0f, // triangle 2 : begin
    4	-1.0f, 1.0f,-1.0f, // triangle 2 : end
    5	1.0f,-1.0f, 1.0f,
    6	1.0f,-1.0f,-1.0f,
    7	1.0f, 1.0f, 1.0f,

	0, 1, 2,
    3, 0, 4,
	5, 0, 6,
    3, 6, 0,
    0, 2, 4,
    5, 1, 0,
    2, 1, 5,
    7, 6, 3,
    6, 7, 5,
    7, 3, 4,
    7, 4, 2,
    7, 2, 5
	*/
	
	//initialize vertices

	float size = 5.0f;

	for(int i=0; i<8; i++){
		vertices.push_back(Vertex());
		//set initial positions
		if(i < 3 || i == 4)
			vertices[i].XYZW[0] = -size;
		else
			vertices[i].XYZW[0] = size;
		if(i < 2 || i == 5 || i == 6)
			vertices[i].XYZW[1] = -size;
		else
			vertices[i].XYZW[1] = size;
		if(i==0 || i == 3 || i==4 || i==6)
			vertices[i].XYZW[2] = -size;
		else
			vertices[i].XYZW[2] = size;
		//everyone is going to have 1 for the W-component
		vertices[i].XYZW[3] = 0;
		vertices[i].NORMAL[0] = vertices[i].NORMAL[1] = vertices[i].NORMAL[2] = 0;
		vertices[i].RGBA[1] = vertices[i].RGBA[2] = 0;
		vertices[i].RGBA[0] = vertices[i].RGBA[3] = 1;
	}

	//now figure out our polies
	for(int k=0; k<12; k++)
		polies.push_back(Poly());

	//a bit of ugly hard coding (I wish I could figure out how to do this 
	//procedurally, but I a pattern didn't immediately jump out at me
	//(may outsource this to just reading from a file later)
	polies[0].indices.push_back(0);
	polies[0].indices.push_back(1);
	polies[0].indices.push_back(2);

	polies[1].indices.push_back(3);
	polies[1].indices.push_back(0);
	polies[1].indices.push_back(4);
	
	polies[2].indices.push_back(5);
	polies[2].indices.push_back(0);
	polies[2].indices.push_back(6);

	polies[3].indices.push_back(3);
	polies[3].indices.push_back(6);
	polies[3].indices.push_back(0);

	polies[4].indices.push_back(4);
	polies[4].indices.push_back(2);
	polies[4].indices.push_back(0);

	polies[5].indices.push_back(5);
	polies[5].indices.push_back(1);
	polies[5].indices.push_back(0);

	polies[6].indices.push_back(2);
	polies[6].indices.push_back(1);
	polies[6].indices.push_back(5);

	polies[7].indices.push_back(7);
	polies[7].indices.push_back(6);
	polies[7].indices.push_back(3);

	polies[8].indices.push_back(6);
	polies[8].indices.push_back(7);
	polies[8].indices.push_back(5);

	polies[9].indices.push_back(7);
	polies[9].indices.push_back(3);
	polies[9].indices.push_back(4);

	polies[10].indices.push_back(7);
	polies[10].indices.push_back(4);
	polies[10].indices.push_back(2);

	polies[11].indices.push_back(7);
	polies[11].indices.push_back(2);
	polies[11].indices.push_back(5);

	//calculate normals here
	calculate_normals();

	prepare_indices();
	prepare_vertices();

	for(int i=0; i<4; i++)
		null_vertex.XYZW[i] = null_vertex.RGBA[i] = -1;
	selected_vertex = &null_vertex;

	cout << "Created object with " << aux_indices.size() << " indices and " << aux_vertices.size() << " vertices\n";

}

OpenGLObject::OpenGLObject(ifstream &input){
	//we're going to start with the name after <Group>
	string token;
	//we want to eat up [name] {
	input >> token >> token >> token;
	while(token != "}"){
		if(token == "<VertexPool>"){
			input >> token >> token >> token;
			while(token != "}"){
				if(token == "<Vertex>"){
					vertices.push_back(Vertex());
					//eat up name and open bracket
					input >> token >> token;
					//store the vertex positions
					for(int i = 0; i < 3; i++){
						input >> token;
						vertices[vertices.size()-1].XYZW[i] = atof(token.c_str());
					}
					vertices[vertices.size() - 1].XYZW[3] = 1.0;
					input >> token;
					//eat up whatever tags until we reach the end
					while(token != "}"){
						//eat up <Normal>
						if(token == "<Normal>"){
							input >> token;
							for(int i = 0; i < 3; i++){
								input >> token;
								vertices[vertices.size()-1].NORMAL[i] = atof(token.c_str());
							}
							input >> token;
						}
						//eat up <RGBA>
						if(token == "<RGBA>"){
							input >> token;
							for(int i = 0; i < 4; i++){
								input >> token;
								vertices[vertices.size()-1].RGBA[i] = atof(token.c_str());
							}
							input >> token;
						}
						input >> token;
					}
				}
				input >> token;
			}
			input >> token;
		}
		if(token == "<Polygon>"){
			polies.push_back(Poly());
			input >> token >> token;
			//we only care about Vertex Ref
			if(token == "<VertexRef>"){
				input >> token;
				for(int i = 0; i < 3; i++){
					input >> token;
					polies[polies.size()-1].indices.push_back(atoi(token.c_str()));
				}
				//eat up <Ref> { Object0.verts } } }
				input >> token >> token >> token >> token >> token;
			}
			//get close bracket
			input >> token;
		}
		input >> token;
	}

	//calculate normals here
	calculate_normals();

	prepare_indices();
	prepare_vertices();

	for(int i=0; i<4; i++)
		null_vertex.XYZW[i] = null_vertex.RGBA[i] = -1;
	selected_vertex = &null_vertex;

	cout << "Created object with " << aux_indices.size() << " indices and " << aux_vertices.size() << " vertices\n";


}

void OpenGLObject::Finish_Rotate(){
	cout << "Updating normals\n";
	calculate_normals();
}

bool OpenGLObject::GetPoly(GLdouble mouse_coords[]){
	//reset old poly to red
	for(int p = 0; p < selected_poly.size(); p++){
		selected_poly[p]->RGBA[0] = 1.0f;
		selected_poly[p]->RGBA[2] = 0.0f;
	}
	
	selected_poly.clear();
	
	//convert mouse_coords to a vec3
	vec3 mouse = TemplateVec(mouse_coords[0], mouse_coords[1], mouse_coords[2]);

	//go poly by poly
	for(int i=0; i < aux_indices.size(); i+=3){
		float angle = 0.0f;
		vec3 lines[3];
		for(int k=0; k<3; k++){
			lines[k] = TemplateVec(aux_vertices[aux_indices[i+k]].XYZW[0], aux_vertices[aux_indices[i+k]].XYZW[1], aux_vertices[aux_indices[i+k]].XYZW[2])
				- mouse;
			lines[k] = NormalizeVec(lines[k]);
		}

		//now add up the angles between the vectors
		for(int a = 0; a < 3; a++){
			//angle = arccos(v1*v2)
			float dot = lines[a] * lines[(a+1)%3];

			float new_ang = acos(dot);
			angle += new_ang;
		}

		float delta = abs(angle - PI*2);
		//cout << "\t\t" << angle << " " << delta << "\n";

		if(delta < 0.2f){
			//cout << "Selected poly " << i/3 << "\n";
			for(int x = 0; x < 3; x++){
				aux_vertices[aux_indices[i+x]].RGBA[0] = 0.0f;
				aux_vertices[aux_indices[i+x]].RGBA[2] = 1.0f;
				selected_poly.push_back(&aux_vertices[aux_indices[i+x]]);
			}
			return true;			
		}

	}
	return false;

}

bool OpenGLObject::GetEdge(GLdouble mouse_coords[]){
	for(int p = 0; p < selected_edge.size(); p++){
		selected_edge[p]->RGBA[0] = 1.0f;
		selected_edge[p]->RGBA[2] = 0.0f;
	}

	selected_edge.clear();

	//find edges
	for(int i=0; i<aux_indices.size(); i+=3){
		//there are three edges to a poly, gen vector for them
		for(int k=0; k<3; k++){
			//parametric method
			vec3 a = TemplateVec(aux_vertices[aux_indices[i+k]].XYZW[0], aux_vertices[aux_indices[i+k]].XYZW[1], aux_vertices[aux_indices[i+k]].XYZW[2]);
			vec3 b = TemplateVec(aux_vertices[aux_indices[i+(k+1)%3]].XYZW[0], aux_vertices[aux_indices[i+(k+1)%3]].XYZW[1], aux_vertices[aux_indices[i+(k+1)%3]].XYZW[2]);

			vec3 line = a - b;
			float t, mouse_x, mouse_y, mouse_z;

			//mouse_coords[0] = a[0] + line[0]*t
			if(line.x != 0){
				t = (mouse_coords[0] - a.x)/line.x;
				mouse_x = mouse_coords[0];
				mouse_y = a.y + line.y*t;
				mouse_z = a.z + line.z*t;
			}
			else if(line.y != 0){
				t = (mouse_coords[1] - a.y)/line.y;
				mouse_x = a.x + line.x*t;
				mouse_y = mouse_coords[1];
				mouse_z = a.z + line.z*t;
			}
			else if(line.z != 0){
				t = (mouse_coords[2] - a.z)/line.z;
				mouse_x = a.x + line.x*t;
				mouse_y = a.y + line.y*t;
				mouse_z = mouse_coords[2];
			}
			else
				continue;

			//cout << t << " ";
			float projected_mouse[3] = {mouse_x, mouse_y, mouse_z};
			float actual_mouse[3] = {mouse_coords[0], mouse_coords[1], mouse_coords[2]};

			if(get_dist(projected_mouse, actual_mouse) < 0.2f){
				cout << "\nAdding an edge\n";
				for(int c = 0; c< 3; c++){
					aux_vertices[aux_indices[i+k]].RGBA[c] = 0.0f;
					aux_vertices[aux_indices[i+(k+1)%3]].RGBA[c] = 0.0f;
				}
				aux_vertices[aux_indices[i+k]].RGBA[2] = 1.0f;
				aux_vertices[aux_indices[i+(k+1)%3]].RGBA[2] = 1.0f;

				selected_edge.push_back(&aux_vertices[aux_indices[i+k]]);
				selected_edge.push_back(&aux_vertices[aux_indices[i+(k+1)%3]]);
				return true;
			}
		}
	}
	return false;

}

//return negative RGBA vals if null
bool OpenGLObject::GetVertex(GLdouble mouse_coords[]){
	if(selected_vertex->RGBA[0] != -1){
		selected_vertex->RGBA[0] = 1.0f;
		selected_vertex->RGBA[2] = 0.0f;
	}

	float cmouse[3];
	for(int k=0; k<3; k++)
		cmouse[k] = mouse_coords[k];

	for(int i=0; i<aux_vertices.size(); i++){
		float dist = get_dist(cmouse, aux_vertices[i].XYZW);
		aux_vertices[i].RGBA[2] = 0.0f;
		if(dist < 2.0f){
			cout << "\t\tClicked on Vertex " << i << "\n";
			aux_vertices[i].RGBA[0] = 0.0f;
			aux_vertices[i].RGBA[2] = 1.0f;
			selected_vertex = &aux_vertices[i];
			return true;
		}
	}

	selected_vertex = &null_vertex;
	return false;
}

bool OpenGLObject::GetObject(GLdouble mouse_coords[]){
	if(GetPoly(mouse_coords)){
		//highlight all vertices in object
		for(int i = 0; i < aux_vertices.size(); i++){
			aux_vertices[i].RGBA[0] = 0;
			aux_vertices[i].RGBA[2] = 1;
		}

		return true;
	}

	return false;
}

void OpenGLObject::prepare_indices(){
	cout << "Preparing indices\n";

	aux_indices.clear();
	for(int i=0; i < polies.size(); i++)
		for(int k = 0; k < polies[i].indices.size(); k++)
			aux_indices.push_back(polies[i].indices[k]);
}

void OpenGLObject::prepare_vertices(){
	cout << "Preparing vertices\n";

	aux_vertices.clear();
	for(int i=0; i < vertices.size(); i++){
		//make a copy of the vertex (original with remain unmodified)
		aux_vertices.push_back(Vertex());
		//set XYZW
		for(int k=0; k< 4; k++)
			aux_vertices[i].XYZW[k] = vertices[i].XYZW[k];
		//set normals
		for(int n = 0; n < 3; n++)
			aux_vertices[i].NORMAL[n] = vertices[i].NORMAL[n];
		//set a color
		for(int c = 0; c < 4; c++)
			aux_vertices[i].RGBA[c] = vertices[i].RGBA[c];
		//aux_vertices[i].RGBA[0] = 1.0f;
		//aux_vertices[i].RGBA[3] = 1.0f;

	}
}

void OpenGLObject::reset_colors(){
	for(int i=0; i < aux_vertices.size(); i++){
		aux_vertices[i].RGBA[0] = 1.0f;
		aux_vertices[i].RGBA[2] = 0.0f;
	}
}

//base transforms
void OpenGLObject::trans_vert(vector <Vertex*> v, float x, float y, float z){
	xvector vec(x, y, z);
	xmatrix transmat(vec);

	for(int i=0; i < v.size(); i++){

		xpoint p(v[i]->XYZW[0], v[i]->XYZW[1], v[i]->XYZW[2]); 
		p = transmat*p;

		//set new values
		for(int k=0; k < 3; k++)
			v[i]->XYZW[k] = p.get_ele(k);

	}

}

void OpenGLObject::rot_vert(vector <Vertex*> v, float h, float p, float r){
	xmatrix rotH("rotateH", h);
	xmatrix rotP("rotateP", p);
	xmatrix rotR("rotateR", r);

	for(int i=0; i < v.size(); i++){
		xpoint pt(v[i]->XYZW[0], v[i]->XYZW[1], v[i]->XYZW[2]); 
		if(h!=0)
			pt = rotH*pt;
		if(p!=0)
			pt = rotP*pt;
		if(r!=0)
			pt = rotR*pt;

		for(int k = 0; k < 3; k++)
			v[i]->XYZW[k] = pt.get_ele(k);
	}

}

void OpenGLObject::scale_vert(vector <Vertex*> v, float s){
	xmatrix transmat("scale", s);
	
	for(int i = 0; i < v.size(); i++){
		xpoint p(v[i]->XYZW[0], v[i]->XYZW[1], v[i]->XYZW[2]);
		p = transmat*p;

		for(int k = 0; k < 3; k++)
			v[i]->XYZW[k] = p.get_ele(k);
	}

}

//translations
void OpenGLObject::translate_vertex(float x, float y, float z){
	//return if we don't have a vertex selected
	if(selected_vertex->RGBA[0] == -1)
		return;
	
	vector <Vertex*> v;
	v.push_back(selected_vertex);

	trans_vert(v, x, y, z);

}

void OpenGLObject::translate_edge(float x, float y, float z){
	trans_vert(selected_edge, x, y, z);

}

void OpenGLObject::translate_poly(float x, float y, float z){
	trans_vert(selected_poly, x, y, z);
}

void OpenGLObject::translate_object(float x, float y, float z){
	vector <Vertex*> v;
	for(int i=0; i < aux_vertices.size(); i++){
		v.push_back(&aux_vertices[i]);
	}

	trans_vert(v, x, y, z);

}

//rotations
void OpenGLObject::rotate_edge(float h, float p, float r){
	rot_vert(selected_edge, h, p, r);
}

void OpenGLObject::rotate_poly(float h, float p, float r){
	rot_vert(selected_poly, h, p, r);
}

void OpenGLObject::rotate_object(float h, float p, float r){
	vector <Vertex*> v;
	for(int i=0; i < aux_vertices.size(); i++)
		v.push_back(&aux_vertices[i]);

	rot_vert(v, h, p, r);
}

//scaling
void OpenGLObject::scale_edge(float s){
	scale_vert(selected_edge, s);
}

void OpenGLObject::scale_poly(float s){
	scale_vert(selected_poly, s);
}

void OpenGLObject::scale_object(float s){
	vector <Vertex*> v;
	for(int i=0; i < aux_vertices.size(); i++)
		v.push_back(&aux_vertices[i]);

	scale_vert(v, s);
}

//split functions
void OpenGLObject::split_edge(){
	//when we split an edge, what we are really doing is adding a vertex and taking
	//tris containing both ends (there should be two) and split them up into four
	
	//if there is no edge selected, return
	if(selected_edge.size() != 2)
		return;

	//first step, add new vertex between the edges
	Vertex new_vert;
	//the new XYZW is going to be the average of the other XYZW's
	for(int i=0; i < selected_edge.size(); i++){
		//if i==0, just set everything as is
		if(i == 0){
			for(int k=0; k < 4; k++){
				new_vert.XYZW[k] = selected_edge[i]->XYZW[k];
				new_vert.RGBA[k] = selected_edge[i]->RGBA[k];
			}
		}
		//else take average
		else{
			for(int k = 0; k < 4; k++){
				new_vert.XYZW[k] = (new_vert.XYZW[k]*i + selected_edge[i]->XYZW[k])/(i+1);
				new_vert.RGBA[k] = (new_vert.RGBA[k]*i + selected_edge[i]->RGBA[k])/(i+1);
			}
		}
	}

	//add this vertex to Vertices and aux_vertices
	vertices.push_back(new_vert);
	aux_vertices.push_back(new_vert);

	//now find the tris that shared this edge and split them up
	vector<int> vertex_indices;
	for(int z = 0; z < aux_vertices.size(); z++){
		for(int y = 0; y < selected_edge.size(); y++){
			cout << "\t comparing vertex ";
			for(int w = 0; w < 3; w++)
				cout << selected_edge[y]->XYZW[w] << " ";
			cout << " to ";
			for(int w = 0; w < 3; w++)
				cout << aux_vertices[z].XYZW[w] << " ";
			cout << "\n";

			if(aux_vertices[z] == *selected_edge[y])
				vertex_indices.push_back(z);
		}
	}

	if(vertex_indices.size() != 2){
		cout << "Vertex indices size " << vertex_indices.size() << " unexpected\n";
		return;
	}

	vector<int> poly_indices;
	for(int x = 0; x < polies.size(); x++){
		bool check1 = false, check2 = false;
		for(int p = 0; p < polies[x].indices.size(); p++){
			//found a match
			if(polies[x].indices[p] == vertex_indices[0] || polies[x].indices[p] == vertex_indices[1]){
				if(!check1)
					check1 = true;
				else
					check2 = true;
			}
		}
		//if both checks passed, add this index
		if(check1 && check2){
			poly_indices.push_back(x);
			cout << "Poly " << x << " shared the selected edge\n";
		}
	}

	//create two new polies that split up each old one, add them and then removeat the old ones
	for(int n = poly_indices.size() - 1; n >= 0; n--){
		for(int m = 0; m < polies[poly_indices[n]].indices.size(); m++){
			//find the vertex that is not part of the edge
			if(polies[poly_indices[n]].indices[m] != vertex_indices[0] && polies[poly_indices[n]].indices[m] != vertex_indices[1]){
				Poly new_poly1, new_poly2;
				//add the non-edge vertex
				new_poly1.indices.push_back(polies[poly_indices[n]].indices[m]);
				new_poly2.indices.push_back(polies[poly_indices[n]].indices[m]);
				//add the new vertex
				new_poly1.indices.push_back(vertices.size()-1);
				new_poly2.indices.push_back(vertices.size()-1);
				//now add the third vertex
				new_poly1.indices.push_back(polies[poly_indices[n]].indices[(m+1)%3]);
				new_poly2.indices.push_back(polies[poly_indices[n]].indices[(m+2)%3]);
				//push back new polies to polies
				polies.push_back(new_poly1);
				polies.push_back(new_poly2);
			}
		}
		//remove the poly at n
		polies.erase(polies.begin() + poly_indices[n]);
	}

	//reset selection
	reset_colors();
	selected_edge.clear();

	//reset indices and calculate new normals
	prepare_indices();
	calculate_normals();
	//prepare_vertices();

}

void OpenGLObject::split_poly(){

}

void OpenGLObject::reset_selections(){
	selected_edge.clear();
	selected_poly.clear();
	selected_vertex = &null_vertex;
}

/*

calculate the normals yourself. For this you take the cross product 
of the vectors spanning each triangle (this is the face normal),
then for each vertex you add the normals of the faces the vertex is 
used in and normalize the resulting vector.

*/

void OpenGLObject::calculate_normals(){
	for(int i = 0; i < polies.size(); i++){
		//calculate triangle vectors
		vec3 v1 = TemplateVec(vertices[polies[i].indices[0]].XYZW[0], vertices[polies[i].indices[0]].XYZW[1], vertices[polies[i].indices[0]].XYZW[2])
			- TemplateVec(vertices[polies[i].indices[1]].XYZW[0], vertices[polies[i].indices[1]].XYZW[1], vertices[polies[i].indices[1]].XYZW[2]);
		vec3 v2 = TemplateVec(vertices[polies[i].indices[1]].XYZW[0], vertices[polies[i].indices[1]].XYZW[1], vertices[polies[i].indices[1]].XYZW[2])
			- TemplateVec(vertices[polies[i].indices[2]].XYZW[0], vertices[polies[i].indices[2]].XYZW[1], vertices[polies[i].indices[2]].XYZW[2]);

		//take cross product of two vectors to get the normal vector
		vec3 normal = CrossProduct(&v1, &v2);
		normal = NormalizeVec(normal);

		//add this normal to the current normal of the vertex
		for(int k = 0; k < 3; k++){
			vertices[polies[i].indices[k]].NORMAL[0] += normal.x;
			vertices[polies[i].indices[k]].NORMAL[1] += normal.y;
			vertices[polies[i].indices[k]].NORMAL[2] += normal.z;
		}
	}
	//normalize all of the vertices
	for(int x = 0; x < vertices.size(); x++){
		vec3 normal = TemplateVec(vertices[x].NORMAL[0], vertices[x].NORMAL[1], vertices[x].NORMAL[2]);
		normal = NormalizeVec(normal);
		vertices[x].NORMAL[0] = normal.x;
		vertices[x].NORMAL[1] = normal.y;
		vertices[x].NORMAL[2] = normal.z;
		if(aux_vertices.size() == vertices.size()){
			aux_vertices[x].NORMAL[0] = normal.x;
			aux_vertices[x].NORMAL[1] = normal.y;
			aux_vertices[x].NORMAL[2] = normal.z;
		}

	}


}
