#include "OpenGLObject.h"
#include <string>

bool exiting = false;
const double radianFactor = 2 * 3.1415926535 / 360; 


//auxiliary structs
struct Button{
	sf::Text btn_text;
	sf::RectangleShape background;
	sf::FloatRect rect;
};
vector<Button> bottom_btns, side_btns, top_btns;
OpenGLObject new_obj;
vector <OpenGLObject> objects;
int object_selection = 0;

// 0 - vertex, 1 - edge, 2 - polygon, 3 - object
int mode = 0;
int control_mode = 0;
int barcode = -1;

void Resize(int x, int y);

//camera stuff
float xrot = 0, yrot = 0, zrot = 0.0;
float old_x, old_y;
GLdouble mouse3Dcoords[3] = {0, 0, 0};

XCamera cam;

//OpenGL Area
/*Vertex *Vertices;
GLuint *Indices;


GLuint
	ProjectionMatrixUniformLocation,
	ViewMatrixUniformLocation,
	ModelMatrixUniformLocation,
	VertexShaderId,
    FragmentShaderId,
    ProgramId,
    VaoId,
    BufferId,
    IndexBufferId;

glm::mat4 ProjectionMatrix = glm::perspective(60.0f, 16.0f/9.0f, 0.1f, 100.f);
glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0, -20, 6), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
glm::mat4 ModelMatrix = glm::mat4(1.0f);
glm::mat3 NormalMatrix = glm::mat3(1.0f);*/

float RotationTimer = 0.0f;

void OnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);
void Initialize(sf::RenderWindow &window);
void CreateShaders();
void CreateVBO();
void checkShader(GLuint);
void RenderFunction(sf::RenderWindow &window);
void DestroyShaders();
void DestroyVBO();
void Cleanup();

//2D Functions
void InitControlPanels(sf::RectangleShape &bottom, sf::RectangleShape &side, sf::RectangleShape &top, sf::RenderWindow &window);
void GUIRender(sf::RenderWindow &window, sf::RectangleShape &bottom, sf::RectangleShape &side, sf::RectangleShape &top);
bool HandleEvents(sf::RenderWindow &window);
void HandleBarcode();
//3D Functions
void AltInit();
void GetMouseCoords(sf::RenderWindow &window, int mouse_x, int mouse_y);
void DrawWorld();
void Handle3DEvents(sf::RenderWindow &window, bool &cooldown, sf::Clock &clock);
//Program Functions
void SaveScene();
void LoadScene();

int main(){
	// Create main window
	sf::RenderWindow window(sf::VideoMode(800, 700), "Sculptor");
	window.EnableVerticalSync(true);

	gluPerspective(40.0, window.GetWidth()/window.GetHeight(), .2, 100);
	bool cooldown = false;

	objects.push_back(new_obj);

	window.SetActive();
	sf::RectangleShape bottom;
	sf::RectangleShape side;
	sf::RectangleShape top;
	InitControlPanels(bottom, side, top, window);
	//Initialize(window);

	//alternate init
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	AltInit();

	old_x = sf::Mouse::GetPosition(window).x;
	old_y = sf::Mouse::GetPosition(window).y;

	//Vertices = &(objects[0].aux_vertices[0]);
	//Indices = &(objects[0].aux_indices[0]);

	sf::Clock clock;

	cam.Move(TemplateVec(0.0, 0.0, 10.0));
	cam.MoveForward(1.0);
	 glMatrixMode(GL_MODELVIEW);

	 float click_cooldown = 0.5f;

	// start game loop
	while(window.IsOpen()){
		//process events
		if(HandleEvents(window)){
			if(control_mode == 0)
				for(int o = 0; o < objects.size(); o++)
					objects[0].reset_colors();
			cooldown = true;
			//check for barcode here
			if(barcode != -1)
				HandleBarcode();

		}
		if(exiting)
			break;

		Handle3DEvents(window, cooldown, clock);

		//display rendered frame on screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		
		// Move the camera
		glLoadIdentity();
		cam.Render();

		DrawWorld();

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		window.PushGLStates();	
		GUIRender(window, bottom, side, top);
		window.PopGLStates();

		//display rendered frame on screen
		window.Display();

		if(cooldown)
			click_cooldown -= clock.GetElapsedTime().AsSeconds();
		if(click_cooldown <= 0.0f){
			cooldown = false;
			click_cooldown = 0.5f;
		}

		clock.Restart();

	}

	cout << "Exiting\n";

	//Cleanup();

	return 0;

}

void HandleBarcode(){
	//save
	if(barcode == 0)
		SaveScene();
	//load
	else if(barcode == 1)
		LoadScene();
	//add cube
	else if(barcode == 2){
		OpenGLObject another_obj;
		another_obj.translate_object(0, objects.size()*10, 0);
		objects.push_back(another_obj);
	}
	//delete cube
	else if(barcode == 3){
		//prompt them to make sure they're sure
		cout << "\n***ARE YOU SURE YOU WANT TO DELETE CURRENTLY SELECTED OBJECT?***\n\n";
		string answer;
		cin >> answer;

		if(answer == "Yes" || answer == "yes" || answer == "y"){
		
			//only let them do it if they have more than one cube on the screen
			if(objects.size() > 1){
				for(int i=0; i < objects.size(); i++)
					objects[i].reset_colors();
				control_mode = 0;
				mode = 0;
				objects.erase(objects.begin() + object_selection);
				object_selection = 0;
			}
		}
	}

	cout << "Registered a barcode click\n";
	barcode = -1;

}

void Handle3DEvents(sf::RenderWindow &window, bool &cooldown, sf::Clock &clock){
	// We get the position of the mouse cursor, so that we can move the box accordingly
		float x =  sf::Mouse::GetPosition(window).x;
		float y =  sf::Mouse::GetPosition(window).y;
     
		float diffx = old_x - x;
		float diffy = old_y - y;

		if(!cooldown && sf::Mouse::IsButtonPressed(sf::Mouse::Left)){
			//make sure we are clicking inside the window
			if(!(x < 0 || y < 0 || x > window.GetWidth() || y > window.GetHeight())){
				//we're going to do transformations relative to our perspective now
				vec3 right = cam.RightVector*-diffx;
				vec3 up = cam.UpVector*diffy;
				vec3 total = up + right;

				//SELECT - this is the only one we're going to set a cooldown for
				if(control_mode == 0){
					GetMouseCoords(window, x, y);
					cout << "3D coords: (" << mouse3Dcoords[0] << ", " << mouse3Dcoords[1] << ", " << mouse3Dcoords[2] << ")\n";
					if(mode == 0)
						objects[object_selection].GetVertex(mouse3Dcoords);
					else if(mode == 1)
						objects[object_selection].GetEdge(mouse3Dcoords);
					else if(mode == 2)
						objects[object_selection].GetPoly(mouse3Dcoords);
					//select object - this is where we could potentially change which object we're working on
					else if(mode == 3){
						for(int z = 0; z < objects.size(); z++){
							//we're going to accept the most original object
							if(objects[z].GetObject(mouse3Dcoords)){
								object_selection = z;
								break;
							}
						}
					}
					cooldown = true;
				}
				//TRANSLATE
				else if(control_mode == 1){
					//vertex
					if(mode == 0)
						objects[object_selection].translate_vertex(total.x, total.y, total.z);//objects[object_selection].translate_edge(-diffx, -diffy, 0);
					else if(mode == 1)
						objects[object_selection].translate_edge(total.x, total.y, total.z);
					else if(mode == 2)
						objects[object_selection].translate_poly(total.x, total.y, total.z);
					else if(mode == 3)
						objects[object_selection].translate_object(total.x, total.y, total.z);
				}
				//ROTATE (not available for single vertex)
				else if(control_mode == 2){
					RotationTimer += clock.GetElapsedTime().AsSeconds();
					cout << RotationTimer << "\n";
					
					//update the thing every second of rotation
					if(RotationTimer > 0.05f){
						RotationTimer = 0.0f;
						objects[object_selection].Finish_Rotate();
					}

					if(mode == 1)
						objects[object_selection].rotate_edge(total.x*PI/6, total.y*PI/6, total.z*PI/6);
					else if(mode == 2)
						objects[object_selection].rotate_poly(total.x*PI/6, total.y*PI/6, total.z*PI/6);//objects[object_selection].rotate_poly(-diffx*PI/6, -diffy*PI/6, 0);
					else if(mode == 3)
						objects[object_selection].rotate_object(total.x*PI/6, total.y*PI/6, total.z*PI/6);
				}
				//note: scale done with mouse wheel
			}
		}
		//rotate camera
		if(sf::Mouse::IsButtonPressed(sf::Mouse::Right)){
			cam.RotateY(0.5*diffx);
			cam.RotateX(0.25*diffy);
		}

		if(!sf::Mouse::IsButtonPressed(sf::Mouse::Left) && RotationTimer > 0){
			cout << "Finished a rotate\n";
			objects[object_selection].Finish_Rotate();
			RotationTimer = 0.0f;
		}

		old_x = x;
		old_y = y;

}

void DrawWorld(){
	//draw world
	glTranslatef(0, 0, -40.f);

	if(mode == 0 || mode == 1)
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL );

	glBegin(GL_TRIANGLES);

	for(int k = 0; k < objects.size(); k++){
		for(int i=0; i<objects[k].aux_indices.size(); i++){
			int index = objects[k].aux_indices[i];
			glColor3f(objects[k].aux_vertices[index].RGBA[0], objects[k].aux_vertices[index].RGBA[1], objects[k].aux_vertices[index].RGBA[2]);
			glVertex3f(objects[k].aux_vertices[index].XYZW[0], objects[k].aux_vertices[index].XYZW[1], objects[k].aux_vertices[index].XYZW[2]);
			glNormal3f(objects[k].aux_vertices[index].NORMAL[0], objects[k].aux_vertices[index].NORMAL[1],objects[k].aux_vertices[index].NORMAL[2]);
		}
	}

	glEnd();

}

/*
//OpenGL Area
void Initialize(sf::RenderWindow &window) {
	
	GLenum GlewInitResult;
    glewExperimental = GL_TRUE;
    GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	glEnable(GL_DEPTH_TEST);
	OnGLError("Depth test");
	glDepthMask(GL_TRUE);

	CreateShaders();
    CreateVBO();
	
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
}

void CreateShaders()
{
    GLenum ErrorCheckValue = glGetError();

	VertexShaderId = LoadShader("light.vs", GL_VERTEX_SHADER);
    glCompileShader(VertexShaderId);
	checkShader(VertexShaderId);

    FragmentShaderId = LoadShader("light.fs", GL_FRAGMENT_SHADER);
    glCompileShader(FragmentShaderId);
	checkShader(FragmentShaderId);
	
    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

	ModelMatrixUniformLocation = glGetUniformLocation(ProgramId, "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ProgramId, "ProjectionMatrix");

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR){
        fprintf(stderr, "ERROR: Could not create the shaders: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
    }
}

void CreateVBO()
{
	OpenGLObject new_obj;

	Vertices = &(new_obj.aux_vertices[0]);
	Indices = &(new_obj.aux_indices[0]);
	
	//Create a VAO and make it active
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    GLenum ErrorCheckValue = glGetError();
    
	const size_t BufferSize = (sizeof(*Vertices))*new_obj.aux_vertices.size();
	const size_t VertexSize = sizeof(*Vertices);
	fprintf(stderr, "Full vertex size: %d\n",sizeof(*Vertices));
	const size_t RGBOffset = sizeof((*Vertices).XYZW);
	fprintf(stderr, "RGB offset: %d\n",sizeof((*Vertices).XYZW));

    glGenBuffers(1, &BufferId);

    glBindBuffer(GL_ARRAY_BUFFER, BufferId);
    glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RGBOffset);


	//Create buffers for vertex indices.
    glGenBuffers(1, &IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*Indices)*new_obj.aux_indices.size(), Indices, GL_STATIC_DRAW);
	
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR){
        fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
    }
}

void checkShader(GLuint shader){
	GLint status;
	//lGetShaderiv gets a particular parameter of the shader
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		int loglen;
		char logbuffer[1000];
		//there's also a corresponding glGetProgramInfoLog function for the linked program object
		glGetShaderInfoLog(shader, sizeof(logbuffer), &loglen, logbuffer);
		fprintf(stderr, "OpenGL Shader Compile Error:\n%.*s", loglen, logbuffer);
	} else {
		int loglen;
		char logbuffer[1000];
		glGetShaderInfoLog(shader, sizeof(logbuffer), &loglen, logbuffer);
		fprintf(stderr, "OpenGL Shader Compile OK:\n%.*s", loglen, logbuffer);
	}
}

GLuint LoadShader(const char* filename, GLenum shader_type){
	GLuint shader_id = 0;
	FILE* file;
	long file_size = -1;
	GLchar* glsl_source;
    fprintf(stderr, "name: %s\n",filename);

	if (NULL != (file = fopen(filename, "rb")) &&
		0 == fseek(file, 0, SEEK_END) &&
		-1 != (file_size = ftell(file))){
		rewind(file);
		if (NULL != (glsl_source = (GLchar*)malloc(file_size + 1))){
			if (file_size == (long)fread(glsl_source, sizeof(GLchar), file_size, file)){
				glsl_source[file_size] = '\0';
                const GLchar* glsl_source_c = glsl_source;
				//fprintf(stderr, "Source: %s\n", glsl_source_c);

				if (0 != (shader_id = glCreateShader(shader_type))){
					glShaderSource(shader_id, 1, &glsl_source_c, NULL);
					glCompileShader(shader_id);
					OnGLError("Could not compile the shader");
				}
				else
					fprintf(stderr, "ERROR: Could not create the shader.\n");
			}
			else
				fprintf(stderr, "ERROR: Could not read file %s\n", filename);

			free(glsl_source);
		}
		else
			fprintf(stderr, "ERROR: Could not allocate %li bytes.\n", file_size);

		fclose(file);
	}
	else
		fprintf(stderr, "ERROR: Could not open file %s\n", filename);

	return shader_id;
}

void OnGLError(const char* error_message){
	const GLenum ErrorValue = glGetError();

	if (ErrorValue != GL_NO_ERROR){
		const char* APPEND_DETAIL_STRING = ": %s\n";
		const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
		const size_t message_length = strlen(error_message);
		char* display_message = (char*)malloc(message_length + APPEND_LENGTH);

		memcpy(display_message, error_message, message_length);
		memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

		fprintf(stderr, display_message, gluErrorString(ErrorValue));

		free(display_message);
	}
}


void RenderFunction(sf::RenderWindow &window){
    window.SetActive();

	glDisable(GL_COLOR_ARRAY);
	glDisable(GL_TEXTURE_COORD_ARRAY);

	glUseProgram(ProgramId);
    OnGLError("DRAW_ERROR: Could not use the shader program");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	OnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(VaoId);
	OnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, 10000, GL_UNSIGNED_INT, (GLvoid*)0);
	OnGLError("ERROR: Could not draw the cube");

}


/*
void DestroyVBO(){
    GLenum ErrorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &BufferId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &IndexBufferId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR){
        fprintf(stderr, "ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
    }
}

void DestroyShaders(){
    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);

    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);

    glDeleteProgram(ProgramId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR){
        fprintf(stderr, "ERROR: Could not destroy the shaders: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
    }
}

void Cleanup()
{
    DestroyShaders();
    DestroyVBO();
}

*/

void Resize(int x, int y){
	if (y == 0 || x == 0) return;  //Nothing is visible then, so return
	
	//Set a new projection matrix
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	//Angle of view:40 degrees
	//Near clipping plane distance: 0.5
	//Far clipping plane distance: 20.0
	gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.2,100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,x,y);  //Use the whole window for rendering
}

bool HandleEvents(sf::RenderWindow &window){
	sf::Event Event;
	bool clicked_on_something = false;

	while(window.PollEvent(Event)){
		//close window : exit
		if(Event.Type == sf::Event::Closed || (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Keyboard::Escape)){
			window.Close();
			exiting = true;
			break;
		}
		// Adjust the viewport when the window is resized
		if (Event.Type == sf::Event::Resized)
			glViewport(0, 0, Event.Size.Width, Event.Size.Height);
		//check for clicks in our control area
		if(Event.Type == sf::Event::MouseButtonPressed){
			for(int i=0; i<bottom_btns.size(); i++){
				if(bottom_btns[i].rect.Contains(Event.MouseButton.X, Event.MouseButton.Y)){
					cout << "Bottom: Registered a click - setting mode to " << i << "\n";
					mode = i;
					control_mode = 0;
					objects[object_selection].reset_selections();
					clicked_on_something = true;
				}
				if(side_btns[i].rect.Contains(Event.MouseButton.X, Event.MouseButton.Y)){
					//don't allow them to click if we're in vertex mode
					if(mode != 0 || (mode == 0 && i <= 1)){
						cout << "Side: Registered a click - setting mode to " << i << "\n";
						control_mode = i;
						clicked_on_something = true;
					}
				}
				if(top_btns[i].rect.Contains(Event.MouseButton.X, Event.MouseButton.Y)){
					barcode = i;
					clicked_on_something = true;
				}
			}
		}
		//affect z-location of whatever we're doing
		if(Event.Type == sf::Event::MouseWheelMoved){
			//we're going to use cam-in for this (wherever that happens to be)
			vec3 total = cam.ViewDir*Event.MouseWheel.Delta;

			//SELECT
			if(control_mode == 0)
				cam.MoveForward(-Event.MouseWheel.Delta);
			//TRANSLATE
			else if(control_mode == 1){
				if(mode == 0)
					objects[object_selection].translate_vertex(total.x, total.y, total.z);//objects[object_selection].translate_vertex(0, 0, Event.MouseWheel.Delta);
				else if(mode == 1)
					objects[object_selection].translate_edge(total.x, total.y, total.z);
				else if(mode == 2)
					objects[object_selection].translate_poly(total.x, total.y, total.z);
				else if(mode == 3)
					objects[object_selection].translate_object(total.x, total.y, total.z);
			}
			//ROTATE
			else if(control_mode == 2){
				if(mode == 1)
					objects[object_selection].rotate_edge(total.x*PI/6, total.y*PI/6, total.z*PI/6);//objects[object_selection].rotate_edge(0, 0, Event.MouseWheel.Delta*PI/6);
				else if(mode == 2)
					objects[object_selection].rotate_poly(total.x*PI/6, total.y*PI/6, total.z*PI/6);
				else if(mode == 3)
					objects[object_selection].rotate_object(total.x*PI/6, total.y*PI/6, total.z*PI/6);
			}
			//SCALE
			else if(control_mode == 3){
				float scale = Event.MouseWheel.Delta;
				if(scale < 0)
					scale = ((100+scale*10)/100);
				else
					scale = scale + .25;
				//cout << "Scale: " << scale << "\n";

				if(mode == 1)
					objects[object_selection].scale_edge(scale);
				else if(mode == 2)
					objects[object_selection].scale_poly(scale);
				else if(mode == 3)
					objects[object_selection].scale_object(scale);
			}
		}
		//nav mode (right mouse button held down)
		if(Event.Type == sf::Event::KeyPressed && sf::Mouse::IsButtonPressed(sf::Mouse::Right)){
			float multiplier = 1;
			if(sf::Keyboard::IsKeyPressed(sf::Keyboard::LShift))
				multiplier = 5;
			if(Event.Key.Code == sf::Keyboard::A)
				cam.StrafeRight(-1.0f*multiplier);
			if(Event.Key.Code == sf::Keyboard::D)
				cam.StrafeRight(1.0f*multiplier);
			if(Event.Key.Code == sf::Keyboard::W)
				cam.MoveForward(-1.0f*multiplier);
			if(Event.Key.Code == sf::Keyboard::S)
				cam.MoveForward(1.0f*multiplier);
		}
		//shortcut keys mode (right mouse button not held down)
		else if(Event.Type == sf::Event::KeyPressed){
			if(Event.Key.Code == sf::Keyboard::Q)
				control_mode = 0;
			if(Event.Key.Code == sf::Keyboard::W)
				control_mode = 1;
			if(Event.Key.Code == sf::Keyboard::E && mode != 0)
				control_mode = 2;
			if(Event.Key.Code == sf::Keyboard::R && mode != 0)
				control_mode = 3;
			//press 't' to split edge
			if(mode == 1 && Event.Key.Code == sf::Keyboard::T)
				objects[object_selection].split_edge();
		}
	}

	return clicked_on_something;
}

void AltInit(){
	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	 // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate inside of jet -> THIS BREAKS SFML CODE

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,128);

    glDepthMask(GL_TRUE);
    glClearDepth(1.f);
	// Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 500.f);
    glColor4f(1.f, 1.f, 1.f, 1.f);

}

void GUIRender(sf::RenderWindow &window, sf::RectangleShape &bottom, sf::RectangleShape &side, sf::RectangleShape &top){
	//draw 2-D stuff
	window.Draw(bottom);
	window.Draw(side);
	window.Draw(top);

	//reset colors, highlight currently selected modes
	for(int i=0; i < bottom_btns.size(); i++){
		if(i==mode)
			bottom_btns[i].background.SetFillColor(sf::Color::Yellow);
		else
			bottom_btns[i].background.SetFillColor(sf::Color::Red);
	}
	for(int k=0; k < side_btns.size(); k++){
		if(k==control_mode)
			side_btns[k].background.SetFillColor(sf::Color::Yellow);
		else
			side_btns[k].background.SetFillColor(sf::Color::Blue);
	}
	for(int j = 0; j < top_btns.size(); j++){
		window.Draw(top_btns[j].background);
		window.Draw(top_btns[j].btn_text);
	}

		
	for(int i=0; i<bottom_btns.size(); i++){
		window.Draw(bottom_btns[i].background);
		window.Draw(bottom_btns[i].btn_text);
	}

	//check for mode and gray out buttons if necessary
	if(mode == 0){
		side_btns[2].background.SetFillColor(sf::Color::White);
		side_btns[3].background.SetFillColor(sf::Color::White);
	}

	for(int k=0; k<side_btns.size(); k++){
		window.Draw(side_btns[k].background);
		window.Draw(side_btns[k].btn_text);
	}

	//if they are in edge mode, let them know that they can hit 't' to split an edge
	if(mode == 1){
		sf::Text split_txt("Press 't' to split edge");
		split_txt.SetPosition(25, 25);
		split_txt.SetColor(sf::Color::White);
		window.Draw(split_txt);
	}
}

void InitControlPanels(sf::RectangleShape &bottom, sf::RectangleShape &side, sf::RectangleShape &top, sf::RenderWindow &window){
	bottom.SetSize(sf::Vector2f(window.GetWidth(), 100));
	bottom.SetOutlineColor(sf::Color::Black);
	bottom.SetOutlineThickness(3);
	bottom.SetFillColor(sf::Color::Blue);
	bottom.SetPosition(0, window.GetHeight() - 100);

	//initialize side bar
	side.SetSize(sf::Vector2f(200, window.GetHeight() - 100));
	side.SetFillColor(sf::Color::Green);
	side.SetPosition(window.GetWidth() - 200, 0);

	//initialize top bar
	top.SetSize(sf::Vector2f(window.GetWidth(), 25));
	top.SetFillColor(sf::Color::Black);
	top.SetPosition(0, 0);

	//make control buttons for bottom bar
	for(int i=0; i<4; i++){
		Button new_btn;
		new_btn.background.SetSize(sf::Vector2f(200, 100));
		new_btn.background.SetFillColor(sf::Color::Red);
		new_btn.background.SetPosition(i*200, window.GetHeight() - 100);
		new_btn.rect = sf::FloatRect(new_btn.background.GetPosition(), new_btn.background.GetSize());
		bottom_btns.push_back(new_btn);
		
	}

	//add the text for buttons
	bottom_btns[0].btn_text = sf::Text(sf::String("Vertex"));
	bottom_btns[1].btn_text = sf::Text(sf::String("Edge"));
	bottom_btns[2].btn_text = sf::Text(sf::String("Polygon"));
	bottom_btns[3].btn_text = sf::Text(sf::String("Object"));

	//now put them in the correct places
	for(int i=0; i<bottom_btns.size(); i++)
		bottom_btns[i].btn_text.SetPosition(i*200 + 50, window.GetHeight() - 75);

	//make control buttons for side bar
	for(int k=0; k<4; k++){
		Button new_btn;
		new_btn.background.SetSize(sf::Vector2f(200, 100));
		new_btn.background.SetFillColor(sf::Color::Blue);
		new_btn.background.SetPosition(window.GetWidth() - 200, k*100);
		new_btn.rect = sf::FloatRect(new_btn.background.GetPosition(), new_btn.background.GetSize());
		side_btns.push_back(new_btn);
	}

	//text and so forth
	side_btns[0].btn_text = sf::Text(sf::String("Select"));
	side_btns[1].btn_text = sf::Text(sf::String("Translate"));
	side_btns[2].btn_text = sf::Text(sf::String("Rotate"));
	side_btns[3].btn_text = sf::Text(sf::String("Scale"));

	for(int k=0; k<side_btns.size(); k++)
		side_btns[k].btn_text.SetPosition(window.GetWidth() - 150, 25 + 100*k);

	//top bar
	for(int i=0; i < 4; i++){
		Button new_btn;
		new_btn.background.SetSize(sf::Vector2f(100, 25));
		new_btn.background.SetFillColor(sf::Color::Black);
		new_btn.background.SetPosition(100*i, 0);
		new_btn.rect = sf::FloatRect(new_btn.background.GetPosition(), new_btn.background.GetSize());
		top_btns.push_back(new_btn);
	}

	top_btns[0].btn_text = sf::Text(sf::String("Save"));
	top_btns[1].btn_text = sf::Text(sf::String("Load"));
	top_btns[2].btn_text = sf::Text(sf::String("Add Cube"));
	top_btns[3].btn_text = sf::Text(sf::String("Del Cube"));

	for(int k = 0; k < top_btns.size(); k++){
		top_btns[k].btn_text.SetPosition(15 + k*100, 5);
		top_btns[k].btn_text.SetCharacterSize(14);
	}
	
}

void GetMouseCoords(sf::RenderWindow &window, int mouse_x, int mouse_y){
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	//get our matrices n' such
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	//translate mouse coords to basic 3D space
	float mouse_z;

	mouse_y = (float)viewport[3] - mouse_y;
	glReadPixels(mouse_x, mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouse_z);

	gluUnProject( mouse_x, mouse_y, mouse_z, modelview, projection, viewport, &mouse3Dcoords[0], &mouse3Dcoords[1], &mouse3Dcoords[2]);

}

void LoadScene(){
	string in_filename;
	cout << "Please enter filename:";
	cin >> in_filename;
	ifstream input(in_filename.c_str(), ios::in); 

	if(!input.is_open())
		return;
	
	//clear our current scene if file open successful
	objects.clear();
	object_selection = 0;

	//read through file and create new objects as we need them
	string token;

	while(!input.eof()){
		input >> token;
		if(token == "<Group>")
			objects.push_back(OpenGLObject(input));
	}
	
	input.close();


}

void SaveScene(){
	//we are going to save each object, all vertices and indices (as close to the maya way as we can)
	string out_filename;
	cout << "Please enter filename to be saved as: ";
	cin >> out_filename;
	ofstream output(out_filename.c_str(), ios::out);

	//take care of the stuff at the beginning
	output << "<CoordinateSystem> { Y-Up }\n\n";

	//now create a new group for each object in scene
	for(int i = 0; i < objects.size(); i++){
		output << "<Group> Object" << i << " {\n";
		output << "\t<VertexPool> Object" << i << ".verts {\n";
		//go through vertices and print them out
		for(int k = 0; k < objects[i].aux_vertices.size(); k++){
			output << "\t\t<Vertex> " << k << " {\n";
			output << "\t\t\t";
			//print out coordinates
			for(int p = 0; p < 3; p++){
				output << objects[i].aux_vertices[k].XYZW[p];
				if(p < 2)
					output << " ";
			}
			//normals
			output << "\n\t\t\t<Normal> {";
			for(int n = 0; n < 3; n++)
				output << " " << objects[i].aux_vertices[k].NORMAL[n];
			//RGBAs
			output << " }\n\t\t\t<RGBA> {";
			for(int r = 0; r < 4; r++)
				output << " " << objects[i].aux_vertices[k].RGBA[r];
			output << " }\n\t\t}\n";
		}
		//end vertex area
		output << "\t}\n";
		//now go through indices and print out polies
		for(int j = 0; j < objects[i].aux_indices.size(); j+=3){
			output << "\t<Polygon> {\n";
			output << "\t\t<VertexRef> {";
			//display indices
			for(int l = 0; l < 3; l++)
				output << " " << objects[i].aux_indices[j+l];
			//finish up the polygon
			output << " <Ref> { Object" << i <<".verts } }\n\t}\n";
		}
		output << "\t}\n";
	}
	output << "}\n";

	output.close();
}