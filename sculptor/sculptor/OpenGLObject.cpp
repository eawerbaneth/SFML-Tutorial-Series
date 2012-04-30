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
} Vertex;

//panda Polygon
struct Poly{
	vector <int> indices;
};