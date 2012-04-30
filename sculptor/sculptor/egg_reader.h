//egg_reader.h

#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include "quaternion.h"
using namespace std;

#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4996)

typedef struct{
    float XYZW[4];
	float RGBA[4];
	float UV[2];
	float normal[3];
} Vertex;

typedef struct{
    float XYZW[4];
    float RGBA[4];
} sVertex;

struct Anim{
	string type;
	vector <float> frames_com;
};

struct Joint{
	string name;

	xmatrix transform;
	vector <string> children;
	string parent;
	vector <int> indices;

	int fps;
	vector <Anim> anims;

};


//maya egg vertex (bendy bar)
typedef struct{
	float XYZW[4];
	float RGBA[4];
	float jWeight[3];
	float uv[2];
	float normal[3];
	int id;
} mVertex;
//note: we can reuse pPoly with this


//panda Vertex
typedef struct{
	float XYZW[4];
	float RGBA[4];
	float uv[2];
	int id;
	//string pool_name;
	//float normal[3];
} pVertex;

//panda Polygon
struct pPoly{
	vector <int> indices;
	string name;
	string pool_name;
	float RGBA[4];
	string tex;
};

//drawable object
class dobject{
public:
	dobject::dobject(){}
	//panda egg
	dobject::dobject(string filename);
	//designate panda or maya
	dobject::dobject(string geometry, string animation);
	void convert_indices(vector <GLuint> &indices);
	void convert_m_indices(vector <GLuint> &indices);
	void convert_vertices(vector <Vertex> &verts);
	void convert_m_vertices(vector <Vertex> &verts);
	pVertex* get_verts(){
		vertices_size = vertices.size();
		return &vertices[0];
	}
	mVertex* get_mVerts(){
		m_vertices_size = mVertices.size();
		return &mVertices[0];
	}
	int* get_indices();
	int* get_mIndices();
	int indices_size;
	int vertices_size;
	int m_vertices_size;
	void scale(float factor);
	void animate(int frame, Vertex* &VERTS_IN_USE);

	vector <Vertex> aux_vertices;
	vector <GLuint> aux_indices;

private:
	vector <pVertex> vertices;
	vector <mVertex> mVertices;
	vector <pPoly> polies;
	vector <Joint> joints;
	void parse_file(ifstream &input);
	void mayaParse(ifstream &input);
	void mayaAnimate(ifstream &input);
	void assign_RGBA();
	int frames;
	vector <mVertex> originals;

	//animation stuff
	void opt_joints();
	Joint* quick_lookup[3];


	//parsing stuff
	//Vertex EatVertex(ifstream &input);
	string EatJoint(ifstream &input, string parent);
	void EatAnimJoint(ifstream &input, int joint_index);
	//pPoly EatPolygon(ifstream &input);
	sf::Clock myclock;
	void make_z_up();
};

void dobject::make_z_up(){
	for(int n = 0; n < mVertices.size(); n++){
		float new_z = mVertices[n].XYZW[1];
		mVertices[n].XYZW[1] = mVertices[n].XYZW[2];
		mVertices[n].XYZW[2] = new_z;
	}

}

float split_joint(char* &token){
	string num = "";
	bool past = false;
	char* c = &token[0];
	while(*c!='\0'){
		if(past)
			num += *c;

		if(*c==':')
			past = true;
		c++;
	}

	return atof(num.c_str());

}

//this will only work with bendy bar
void dobject::opt_joints(){
	for(int i=0; i<joints.size(); i++){
		if(joints[i].name == "joint1")
			quick_lookup[0] = &joints[i];
		if(joints[i].name == "joint2")
			quick_lookup[1] = &joints[i];
		if(joints[i].name == "joint3")
			quick_lookup[2] = &joints[i];
	}
}

xpoint rotate(float h, float p, float r, xpoint coords){
	/*xquat headingquat(xvector(1, 0, 0), h);
	xquat pitchquat(xvector(0, 1, 0), p);
	xquat rollquat(xvector(0, 0, 1), r);

	xquat finalquat = headingquat*pitchquat;
	finalquat = finalquat*rollquat;

	xmatrix rotmat = finalquat.convert_to_matrix();
	
	return rotmat*coords;*/

	xmatrix rotH("rotateH", h);
	xmatrix rotP("rotateP", p);
	xmatrix rotR("rotateR", r);
	coords = rotH*coords;
	coords = rotP*coords;
	return rotR*coords;
		
}

xpoint translateXYZ(float x, float y, float z, xpoint coords){
	xvector vec(x, y, z);
	xmatrix transmat(vec);
	return transmat*coords;
}

void dobject::animate(int sys_frame, Vertex* &VERTS_IN_USE){
	float elapsed = myclock.GetElapsedTime().AsMilliseconds();
	//cout << elapsed << "\n";
	if(elapsed > 1000.0f/24){
		myclock.Restart();
		frames++;
	}
	
	for(int i=0; i<mVertices.size(); i++){
		vector<float> XYZ(3, 0);
		vector<float> HPR(3, 0);
		xpoint coords(mVertices[i].XYZW[0], mVertices[i].XYZW[1], mVertices[i].XYZW[2]);
		//transform by animation at this frame * membership
		for(int k=0; k<3; k++){
			int frame = frames % quick_lookup[k]->fps;
			//frame is 0, reset to original position
			if(frame == 0){
				for(int o=0; o<3; o++){
					mVertices[i].XYZW[o] = originals[i].XYZW[o];
					coords.my_point[o] = originals[i].XYZW[o];
				}
				//break;
			}
			//just record anim transform, math comes at the end
			for(int j=0; j<quick_lookup[k]->anims.size(); j++){
				if(quick_lookup[k]->anims[j].type == "x"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						XYZ[0] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						XYZ[0] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];
				}
				if(quick_lookup[k]->anims[j].type == "z"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						XYZ[1] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						XYZ[1] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];

				}
				if(quick_lookup[k]->anims[j].type == "y"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						XYZ[2] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						XYZ[2] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];
				}
				if(quick_lookup[k]->anims[j].type == "r"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						HPR[0] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						HPR[0] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];
				}
				if(quick_lookup[k]->anims[j].type == "h"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						HPR[1] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						HPR[1] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];
				}
				if(quick_lookup[k]->anims[j].type == "p"){
					if(quick_lookup[k]->anims[j].frames_com.size()>1)
						HPR[2] += quick_lookup[k]->anims[j].frames_com[frame]*mVertices[i].jWeight[k];
					else
						HPR[2] += quick_lookup[k]->anims[j].frames_com[0]*mVertices[i].jWeight[k];
				}

			}
		}

		//ONLY DO THE MATH ONCE FOR EACH VERTEX

		//convert HPR to radians
		for(int x=0; x<3; x++)
			HPR[x] = HPR[x]*0.0174532925;

		//coords.print();
		coords = rotate(HPR[0], HPR[1], HPR[2], coords);
		coords = translateXYZ(XYZ[0], XYZ[1], XYZ[2], coords);
		//cout << "\t";
		//if(i==60)
		//	coords.print();

		//also need to update the vertices that opengl is using
		for(int y=0; y<4; y++){
			//mVertices[i].XYZW[y] = coords.get_ele(y);
			VERTS_IN_USE[i].XYZW[y] = coords.get_ele(y);
		}

	}
}


vector <vector <float>> row_to_col(vector <vector <float>> row_major){
	vector <vector <float>> col_major;

	for(int x = 0; x< 4; x++){
		vector<float> new_row;
		for(int y = 0; y<4; y++)
			new_row.push_back(0.0f);
		col_major.push_back(new_row);

	}

	for(int i=0; i<4; i++){
		for(int k=0; k<4; k++){
			col_major[i][k] = row_major[k][i];
		}
	}

	return col_major;

}

void dobject::mayaParse(ifstream &input){
	//keep track of how deep into brackets we are
	int level = 0;
	int pool = -1;
	string pool_name;
	int pool_level = 0;
	vector <mVertex> pool_vec;
	int loop = 0;

	while(!input.eof()){
		//retrieve next item
		string temp;
		input >> temp;

		//CHECK FOR TYPE
		//check for close bracket
		if(temp == "}"){
			level--;
		}
		//check for ignored open bracket
		if(temp == "{"){
			level++;
		}
		//if it's group, explore further
		else if(temp == "<Group>"){
			//advance to next open bracket
			while(temp != "{")
				input >> temp;
			level++;
		}
		//if it's a vertex pool, save name and explore further
		else if(temp == "<VertexPool>"){
			//get name of pool
			input >> temp;
			pool++;
			pool_name = temp;
			pool_level = level;
			//get rid of open bracket
			input >> temp;
			pool_vec.clear();
			level++;
		}
		//if it's a polygon, eat it whole
		else if(temp == "<Polygon>"){
			pPoly newpoly;
			//eat the open bracket
			input >> temp;
			//go until we reach the end of this poly
			while(temp != "}"){
				//eat up RGBA tags
				if(temp == "<RGBA>"){
					//eat open bracket
					input >> temp;
					input >> newpoly.RGBA[0] >> newpoly.RGBA[1] >> newpoly.RGBA[2] >> newpoly.RGBA[3];
					//eat close bracket
					input >> temp;
				}
				//eat up TRef tags
				else if(temp == "<TRef>"){
					input >> temp;//eat open bracket
					input >> newpoly.tex;
					input >> temp;//eat close bracket
				}
				//eat up VertexRef tags
				else if(temp == "<VertexRef>"){
					input >> temp;//eat open bracket
					//eat up all the numbers until reach a <Ref> tag
					input >> temp;
					while(temp != "<Ref>"){
						newpoly.indices.push_back(atoi(temp.c_str()));
						input >> temp;
					}
					//eat Ref's open bracket and get name
					input >> temp >> temp;
					newpoly.pool_name = temp;
					//eat closing brackets for both <Ref> and <VertexRef>
					input >> temp >> temp;
				}
				//ignore any other tags
				else if(temp.length() > 0 && temp[0] == '<'){
					//eat until we get the close bracket
					while(temp != "}")
						input >> temp;
				}
				input >> temp;
			}
			polies.push_back(newpoly);
		}
		//if it's a vertex, eat it whole
		else if(temp == "<Vertex>"){
			mVertex newvert;
			//get the Vertex's id
			input >> newvert.id;
			//eat the open bracket
			input >> temp;
			//get the coordinates
			input >> newvert.XYZW[0] >> newvert.XYZW[1] >> newvert.XYZW[2];
			newvert.XYZW[3] = 1;
			//go until we reach the end of this vertex
			while(temp != "}"){
				//eat up normal tags
				if(temp == "<Normal>"){
					//eat open bracket
					input >> temp;
					//float normal;
					input >> newvert.normal[0] >> newvert.normal[1] >> newvert.normal[2];
					//input >> normal >> normal >> normal;
					//eat close bracket
					input >> temp;
				}
				//eat up UV tags
				else if(temp == "<UV>"){
					input >> temp;//eat open bracket
					input >> newvert.uv[0] >> newvert.uv[1];
					input >> temp;//eat close bracket
				}
				//eat up RGBA tags
				else if(temp == "<RGBA>"){
					input >> temp; //eat open bracket
					input >> newvert.RGBA[0] >> newvert.RGBA[1] >> newvert.RGBA[2] >> newvert.RGBA[3];
					input >> temp;//eat close bracket

				}
				//eat up joints
				else if(temp == "//"){
					char line[1024];
					input.getline(line, 1024);
					char* tok;
					tok = strtok(line, " ");
					int joint = 0;
					while(tok != NULL){
						//cout << "\tConverting " << tok << " to float " << split_joint(tok) << "\n";
						newvert.jWeight[joint] = split_joint(tok);

						joint++;
						tok = strtok(NULL, " ");
					}					
				}
				//ignore any other tags
				else if(temp.length() > 0 && temp[0] == '<'){
					//eat until we get the close bracket
					while(temp != "}")
						input >> temp;
				}
				input >> temp;
			}
			mVertices.push_back(newvert);

		}
		//if it's a joint, eat it up
		else if(temp == "<Joint>"){
			EatJoint(input, "none");
		}
		//ignore everything else
		

	}

	//assign_RGBA();

}

void dobject::mayaAnimate(ifstream &input){
	string temp;
	input >> temp;
	while( !input.eof() ){
		if(temp == "<Table>"){
			//see if it has a name
			input >> temp;
			//not named, just continue
			if(temp == "{")
				continue;
			//named, check to see if it contains a joint
			else{
				for(int i=0; i<joints.size(); i++){
					if(temp == joints[i].name)
						EatAnimJoint(input, i);
				}
			}
		}
		//we don't care about any other tags in here

		input >> temp;
	}

}

void dobject::EatAnimJoint(ifstream &input, int joint_index){
	string temp;
	//eat open bracket
	input >> temp;
	//go until we get to the close bracket
	while(temp != "}"){
		//frame animation tags
		if(temp == "<Xfm$Anim_S$>"){
			//eat up xform and open bracket
			input >> temp >> temp;
			while(temp != "}"){
				//fps tag
				if(temp == "<Scalar>"){
					//eat up fps { [number]
					input >> temp >> temp >> temp;
					//store number
					joints[joint_index].fps = atoi(temp.c_str());
					//consume close bracket
					input >> temp;
				}
				//done care about this but need to eat it up anyway
				if(temp == "<Char*>"){
					//eating order { [order] }
					input >> temp >> temp >> temp >> temp;
				}
				//animate tag
				if(temp == "<S$Anim>"){
					//make a new anim struct, store its type
					Anim new_anim;
					input >> temp;
					new_anim.type = temp;
					//eat up open bracket <V> open bracket
					input >> temp >> temp >> temp;
					input >> temp;
					//until we reach a close bracket, add numbers to frames_com
					while(temp!= "}"){
						new_anim.frames_com.push_back(atof(temp.c_str()));
						input >> temp;
					}
					//store it
					joints[joint_index].anims.push_back(new_anim);
					//eat the final close bracket
					input >> temp;
				}
				input >> temp;
			}
		}
		//another joint
		if(temp == "<Table>"){
			//get its name
			input >> temp;
			for(int i=0; i<joints.size(); i++){
				if(joints[i].name == temp)
					EatAnimJoint(input, i);
			}
		}

		input >> temp;
	}
}

/*
void EatVertex(ifstream &input){

}*/

string dobject::EatJoint(ifstream &input, string parent){
	string temp;
	//WRITE ME!
	cout << "Found a joint\n";
	//Syntax: <Joint> name { [transform] [ref-list] [joint-list] }
	Joint new_joint;
	//eat up name
	input >> new_joint.name;
	new_joint.parent = parent;
	//eat up open bracket
	input >> temp;
	while(temp != "}"){
		//eat it whole and store the transform matrix
		if(temp == "<Transform>"){
			//eat up open bracket, <Matrix4>, open bracket
			input >> temp >> temp >> temp;
			//remember: this is in row-major format
			vector <vector <float>> row_major;
			for(int row = 0; row < 4; row++){
				vector <float> new_row;
				for(int col = 0; col < 4; col++){
					float f;
					input >> f;
					new_row.push_back(f);
				}
				row_major.push_back(new_row);
			}
			//convert that guy to column major and store it
			new_joint.transform = xmatrix(row_to_col(row_major), "transform");
			//eat up both close brackets
			input >> temp >> temp;
		}
		//found a child joint, eat it up whole
		if(temp == "<Joint>"){
			new_joint.children.push_back(EatJoint(input, new_joint.name));
		}
		//eat up VertexRef
		if(temp == "<VertexRef>"){
			//eat up open bracket
			input >> temp;
			//next line is going to be vertices, eat it up
			char line[108];
			input.getline(line, 108);
			input.getline(line, 108);
			//strtok that guy!
			char* tok;
			tok = strtok(line, " ");
			while(tok != NULL){
				new_joint.indices.push_back(atoi(tok));
				tok = strtok(NULL, " ");
			}
			char templine[1024];
			//eat up next two lines
			input.getline(templine, 1024);
			input.getline(templine, 1024);
			//eat up close bracket
			input >> temp;
		}

		input >> temp;
	}

	//add this joint to joints
	joints.push_back(new_joint);

	return new_joint.name;
}

/*
void EatPolygon(ifstream &input){
	
}*/

dobject::dobject(string geometry, string animation){
	frames = 0;
	myclock.Restart();

	ifstream geo(geometry.c_str());
	if(!geo.is_open()){
		cout << "Error: could not open file\n";
		return;
	}

	mayaParse(geo);
	cout << mVertices.size() << " vertices and " << polies.size() << " polies found\n";
	
	ifstream ani(animation.c_str());
	if(!ani.is_open()){
		cout << "Error: could not open animation file\n";
		return;
	}

	mayaAnimate(ani);
	cout << "Done parsing animation file";

	opt_joints();
	make_z_up();

	for(int i=0; i<mVertices.size(); i++)
		originals.push_back(mVertices[i]);

}




//scale the panda
void dobject::scale(float factor){
	xmatrix transmat("scale", factor);
	for(int i=0; i<vertices.size(); i++){
		xpoint point(vertices[i].XYZW[0], vertices[i].XYZW[1], vertices[i].XYZW[2]);
		point = transmat*point;
		for(int k=0; k<4; k++)
			vertices[i].XYZW[k] = point.get_ele(k);
	}
}

void dobject::convert_vertices(vector <Vertex> &verts){
	for(int i=0; i < vertices.size(); i++){
		Vertex new_vert;
		for(int x=0; x<4; x++)
			new_vert.XYZW[x] = vertices[i].XYZW[x];
		for(int y=0; y<2; y++)
			new_vert.UV[y] = vertices[i].uv[y];
		verts.push_back(new_vert);
	}
	vertices_size = verts.size();
}

void dobject::convert_m_vertices(vector <Vertex> &verts){
	aux_vertices.clear();
	for(int i=0; i < mVertices.size(); i++){
		Vertex new_vert;
		for(int x=0; x<4; x++)
			new_vert.XYZW[x] = mVertices[i].XYZW[x];
		for(int y=0; y<4; y++)
			new_vert.RGBA[y] = mVertices[i].RGBA[y];
		for(int n=0; n<3; n++)
			new_vert.normal[n] = mVertices[i].normal[n];
		new_vert.UV[0] = new_vert.UV[1] = 0.2f;
		verts.push_back(new_vert);
		aux_vertices.push_back(new_vert);
	}
	m_vertices_size = verts.size();
}

int* dobject::get_mIndices(){
	vector <int> indices;
	for(int i=0; i < polies.size(); i++)
		for(int k = 0; k<polies[i].indices.size(); k++)
			indices.push_back(polies[i].indices[k]);
	indices_size = indices.size();

	return &indices[0];
}

int* dobject::get_indices(){
	vector <int> indices;
	for(int i=0; i < polies.size(); i++)
		for(int k = 0; k<polies[i].indices.size(); k++)
			indices.push_back(polies[i].indices[k]-1);
	indices_size = indices.size();

	return &indices[0];
}

void dobject::convert_indices(vector <GLuint> &indices){
	for(int i=0; i < polies.size(); i++)
		for(int k = 0; k<polies[i].indices.size(); k++)
			indices.push_back((GLuint)polies[i].indices[k]-1);
	indices_size = indices.size();
}

void dobject::convert_m_indices(vector <GLuint> &indices){
	aux_indices.clear();
	for(int i=0; i < polies.size(); i++)
		for(int k = 0; k<polies[i].indices.size(); k++){
			indices.push_back((GLuint)polies[i].indices[k]);
			aux_indices.push_back((GLuint)polies[i].indices[k]);
		}
	indices_size = indices.size();
}

void dobject::assign_RGBA(){
	for(int i=0; i<polies.size(); i++){
		for(int k=0; k<polies[i].indices.size(); k++){
			for(int j=0; j<4; j++)
				vertices[polies[i].indices[k]-1].RGBA[j] = polies[i].RGBA[j];
		}
	}
}

void dobject::parse_file(ifstream &input){
	//keep track of how deep into brackets we are
	int level = 0;
	int pool = -1;
	string pool_name;
	int pool_level = 0;
	vector <pVertex> pool_vec;
	int loop = 0;

	while(!input.eof()){
		//retrieve next item
		string temp;
		input >> temp;

		//CHECK FOR TYPE
		//check for close bracket
		if(temp == "}"){
			level--;
		}
		//check for ignored open bracket
		if(temp == "{"){
			level++;
		}
		//if it's group, explore further
		else if(temp == "<Group>"){
			//advance to next open bracket
			while(temp != "{")
				input >> temp;
			level++;
		}
		//if it's a vertex pool, save name and explore further
		else if(temp == "<VertexPool>"){
			//get name of pool
			input >> temp;
			pool++;
			pool_name = temp;
			pool_level = level;
			//get rid of open bracket
			input >> temp;
			pool_vec.clear();
			level++;
		}
		//if it's a polygon, eat it whole
		else if(temp == "<Polygon>"){
			pPoly newpoly;
			//eat the open bracket
			input >> temp;
			//go until we reach the end of this poly
			while(temp != "}"){
				//eat up RGBA tags
				if(temp == "<RGBA>"){
					//eat open bracket
					input >> temp;
					input >> newpoly.RGBA[0] >> newpoly.RGBA[1] >> newpoly.RGBA[2] >> newpoly.RGBA[3];
					//eat close bracket
					input >> temp;
				}
				//eat up TRef tags
				else if(temp == "<TRef>"){
					input >> temp;//eat open bracket
					input >> newpoly.tex;
					input >> temp;//eat close bracket
				}
				//eat up VertexRef tags
				else if(temp == "<VertexRef>"){
					input >> temp;//eat open bracket
					//eat up all the numbers until reach a <Ref> tag
					input >> temp;
					while(temp != "<Ref>"){
						newpoly.indices.push_back(atoi(temp.c_str()));
						input >> temp;
					}
					//eat Ref's open bracket and get name
					input >> temp >> temp;
					newpoly.pool_name = temp;
					//eat closing brackets for both <Ref> and <VertexRef>
					input >> temp >> temp;
				}
				//ignore any other tags
				else if(temp.length() > 0 && temp[0] == '<'){
					//eat until we get the close bracket
					while(temp != "}")
						input >> temp;
				}
				input >> temp;
			}
			polies.push_back(newpoly);
		}
		//if it's a vertex, eat it whole
		else if(temp == "<Vertex>"){
			pVertex newvert;
			//get the Vertex's id
			input >> newvert.id;
			//eat the open bracket
			input >> temp;
			//get the coordinates
			input >> newvert.XYZW[0] >> newvert.XYZW[1] >> newvert.XYZW[2];
			newvert.XYZW[3] = 1;
			//go until we reach the end of this vertex
			while(temp != "}"){
				//eat up normal tags
				if(temp == "<Normal>"){
					//eat open bracket
					input >> temp;
					float normal;
					//input >> newvert.normal[0] >> newvert.normal[1] >> newvert.normal[2];
					input >> normal >> normal >> normal;
					//eat close bracket
					input >> temp;
				}
				//eat up UV tags
				else if(temp == "<UV>"){
					input >> temp;//eat open bracket
					input >> newvert.uv[0] >> newvert.uv[1];
					input >> temp;//eat close bracket
				}
				//ignore any other tags
				else if(temp.length() > 0 && temp[0] == '<'){
					//eat until we get the close bracket
					while(temp != "}")
						input >> temp;
				}
				input >> temp;
			}
			vertices.push_back(newvert);

		}
		//ignore everything else
		

	}

	//assign_RGBA();

}

dobject::dobject(string filename){
	ifstream input(filename.c_str());
	if(!input.is_open()){
		cout << "Error: could not open file\n";
		return;
	}


	parse_file(input);


	cout << vertices.size() << " vertices and " << polies.size() << " polies found\n";


}