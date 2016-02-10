#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
GLuint programID;

/*********************************************************************************************/


const int BOARDWIDTH=20;
const int BOARDLENGTH=20;


int BOARD_MAP[BOARDLENGTH][BOARDWIDTH] ={
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0},
	{0,0,0,2,2,2,2,0,1,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0},
	{1,1,1,1,0,0,0,0,0,2,2,2,1,1,0,0,0,0,0,0},
	{1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0},
	{1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};


typedef struct cubeColour{
	int c1,c2,c3,c4,c5,c6;
}cubeColour;




float camera_rotation_angle=0;
float camera_angle=0;
int isincrementleft=0;
int isincrementright=0;
int isincrementdown=0;
int isincrementup=0;
int colour_state=0;

float MAN_POS_X=2.0*1.6*((BOARDLENGTH-1)-BOARDLENGTH/2.0);
float MAN_POS_Y=-2.0*1.6*BOARDWIDTH/2.0;
float MAN_POS_Z =0.4;
float MAN_VEL_Z=0.3;

float DECELERATION=MAN_VEL_Z/15.0;
float thigh_angle=0.0;
int thigh_direction=1;
bool isjump=0;
bool ismanincrement=0;
bool ismanrotate=0;
int  man_rotatedirection=1;
int man_movedirection=1;
float man_angle = 0;

vector<pair<float,float> > blockcenters;
vector<bool> rotate_status;
vector<pair<float,float> > woodbrickcenters;
vector<bool> brick_rotate_status;
int brick_fall_status[1000];
float brick_height[1000];


bool firsttime=true;
bool isfall=false;


float leg_angle =0;
int leg_direction=1;


VAO * BLOCKS[1000000],*LOWER_BODY,*UPPER_BODY,*LEGS[2],*THIGHS1,*SHOE[2];
VAO *THIGHS[2],*NECK,*UPPER_HEAD,*LOWER_HEAD,*MIDDLE_HEAD,*SHOULDER[2],*HANDS[2];
VAO * WOODBRICK_1;



/*******************************************************************************************/


float ExtractRed(int x)
{
	return ((float)x/1000000.0)/255.0;
}
float ExtractGreen(int x)
{
	x%=1000000;
	return ((float)x/1000.0)/255.0;
}

float ExtractBlue(int x)
{
	x%=1000;
	return (float)x/255.0;

}


void check_fall(void)
{
	int flag=0;
	for(int i=0;i<blockcenters.size();i++)
	{
		if(blockcenters[i].first-1.6<=MAN_POS_X && blockcenters[i].first+1.6>=MAN_POS_X && MAN_POS_Y>=blockcenters[i].second-1.6 && MAN_POS_Y<=blockcenters[i].second+1.6 && MAN_POS_Z>=0.4)
	//	if(pow((blockcenters[i].first-MAN_POS_X),2)+pow(blockcenters[i].second-MAN_POS_Y,2)<=1 && MAN_POS_Z>=0.4)
		{
			flag=1;
		}
	}
	if(!flag)
		isfall=true;
	else 
		isfall=false;

}






/***************************************************************************************/



GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;
	glGenVertexArrays(1, &(vao->VertexArrayID)); 
	glGenBuffers (1, &(vao->VertexBuffer));
	glGenBuffers (1, &(vao->ColorBuffer));
	glBindVertexArray (vao->VertexArrayID);
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	return vao;
}

struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

void draw3DObject (struct VAO* vao)
{
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
	glBindVertexArray (vao->VertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_C:
				break;
			case GLFW_KEY_UP:
				isincrementup=0;
				break;
			case GLFW_KEY_DOWN:
				isincrementdown=0;
				break;
			case GLFW_KEY_LEFT:
				isincrementleft=0;
				break;
			case GLFW_KEY_RIGHT:
				isincrementright=0;
				break;
			case GLFW_KEY_W:
				ismanincrement=0;
				break;
			case GLFW_KEY_S:
				ismanincrement=0;
				break;
			case GLFW_KEY_A:
				ismanrotate=0;
				break;
			case GLFW_KEY_D:
				ismanrotate=0;
				break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			case GLFW_KEY_SPACE:
				isjump=1;
				break;
			case GLFW_KEY_UP:
				isincrementup=1;
				break;
			case GLFW_KEY_DOWN:
				isincrementdown=1;
				break;
			case GLFW_KEY_LEFT:
				isincrementleft=1;
				break;
			case GLFW_KEY_RIGHT:
				isincrementright=1;
				break;
			case GLFW_KEY_W:
				man_movedirection=1;
				ismanincrement=1;
				break;
			case GLFW_KEY_S:
				man_movedirection=-1;
				ismanincrement=1;
				break;
			case GLFW_KEY_A:
				man_rotatedirection=1;
				ismanrotate=1;
				break;
			case GLFW_KEY_D:
				man_rotatedirection=-1;
				ismanrotate=1;
				break;

			default:
				break;
		}
	}
}

void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}

void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_RELEASE)
			{
			}
			else 
			{
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_RELEASE) {
			}
			else
			{
			}
			break;
		default:
			break;
	}
}

void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);
	GLfloat fov = 90.0f;
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
	Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);
	//Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

//same tells that if all colurs are same.
VAO * createCube(cubeColour X,int same,float x1=-1.5,float  y1=-1.5,float z1=0 , float x2=1.5,float y2=-1.5,float z2=0 ,float x3=-1.5,float y3=-1.5,float z3=1 ,float x4=1.5,float y4=-1.5,float z4=1 ,float x5=-1.5,float y5=1.5,float z5=1 ,float x6=1.5,float y6=1.5,float z6=1 ,float x7=-1.5,float y7=1.5,float z7=0,float x8=1.5,float y8=1.5,float z8=0)
	
{
	VAO * temp_cube;
	if(same)
	{
		X.c2=X.c1;
		X.c3=X.c1;
		X.c4=X.c1;
		X.c5=X.c1;
		X.c6=X.c1;
	}
	//vertices from bottom out ,up out ,up in ,bottom in
	/*	float x1=-1;float y1=-1;float z1=0;
		float x2=1;float y2=-1;float z2=0;

		float x3=-1;float y3=-1;float z3=1;
		float x4=1;float y4=-1;float z4=1;

		float x5=-1;float y5=1;float z5=1;
		float x6=1;float y6=1; float z6=1;

		float x7=-1;float y7=1;float z7=0;
		float x8=1;float y8=1;float z8=0;
		*/	
	//colours
	float r1=ExtractRed(X.c1);float g1=ExtractGreen(X.c1);float b1=ExtractBlue(X.c1);
	float r2=ExtractRed(X.c2);float g2=ExtractGreen(X.c2);float b2=ExtractBlue(X.c2);
	float r3=ExtractRed(X.c3);float g3=ExtractGreen(X.c3);float b3=ExtractBlue(X.c3);
	float r4=ExtractRed(X.c4);float g4=ExtractGreen(X.c4);float b4=ExtractBlue(X.c4);
	float r5=ExtractRed(X.c5);float g5=ExtractGreen(X.c5);float b5=ExtractBlue(X.c5);
	float r6=ExtractRed(X.c6);float g6=ExtractGreen(X.c6);float b6=ExtractBlue(X.c6);

	GLfloat g_vertex_buffer_data[] = {
		x1,y1,z1,//1
		x3,y3,z3,//-1.0f,-1.0f, 1.0f,                  //- 1 -1 -1 x1
		x5,y5,z5,

		x8,y8,z8,//1.0f, 1.0f,-1.0f,//2   /////////////
		x1,y1,z1,
		x7,y7,z7,

		x4,y4,z4,//3
		x1,y1,z1,
		x2,y2,z2,

		x8,y8,z8,//2  //////////////
		x2,y2,z2,
		x1,y1,z1,

		x1,y1,z1,//1
		x5,y5,z5,
		x7,y7,z7,

		x4,y4,z4,//3
		x3,y3,z3,//-1.0f,-1.0f, 1.0f,
		x1,y1,z1,

		x5,y5,z5,//4
		x3,y3,z3,//-1.0f,-1.0f, 1.0f,
		x4,y4,z4,

		x6,y6,z6,//5
		x2,y2,z2,
		x8,y8,z8, ////////////////

		x2,y2,z2,//5
		x6,y6,z6,
		x4,y4,z4,

		x6,y6,z6,//6
		x8,y8,z8, //////////////
		x7,y7,z7,

		x6,y6,z6,//6
		x7,y7,z7,
		x5,y5,z5,

		x6,y6,z6,//4
		x5,y5,z5,
		x4,y4,z4
	};


	GLfloat g_color_buffer_data[] = {
		r1,g1,b1,
		r2,g2,b2,
		r3,g3,b3,

		r4,g4,b4,
		r1,g1,b1,
		r4,g4,b4,

		r2,g2,b2,
		r1,g1,b1,
		r1,g1,b1,

		r4,g4,b4,
		r1,g1,b1,
		r1,g1,b1,

		r1,g1,b1,
		r3,g3,b3,
		r4,g4,b4,

		r2,g2,b2,
		r2,g2,b2,
		r1,g1,b1,

		r3,g3,b3,
		r2,g2,b2,
		r2,g2,b2,

		r3,g3,b3,    
		r1,g1,b1,    
		r4,g4,b4,    

		r1,g1,b1,    
		r3,g3,b3,    
		r2,g2,b2,    

		r3,g3,b3,
		r4,g4,b4,
		r4,g4,b4,

		r3,g3,b3,
		r4,g4,b4,
		r3,g3,b3,

		r3,g3,b3,
		r3,g3,b3,
		r2,g2,b2,
	};

	GLfloat g_color_buffer_data1[] = {

		r3,g3,b3,
		r3,g3,b3,
		r3,g3,b3,

		r4,g4,b4,
		r4,g4,b4,
		r4,g4,b4,

		r5,g5,b5,
		r5,g5,b5,
		r5,g5,b5,

		r4,g4,b4,
		r4,g4,b4,
		r4,g4,b4,

		r3,g3,b3,
		r3,g3,b3,
		r3,g3,b3,

		r5,g5,b5,
		r5,g5,b5,
		r5,g5,b5,

		r2,g2,b2,
		r2,g2,b2,
		r2,g2,b2,

		r1,g1,b1,
		r1,g1,b1,
		r1,g1,b1,

		r1,g1,b1,
		r1,g1,b1,
		r1,g1,b1,

		r6,g6,b6,
		r6,g6,b6,
		r6,g6,b6,

		r6,g6,b6,
		r6,g6,b6,
		r6,g6,b6,

		r2,g2,b2,
		r2,g2,b2,
		r2,g2,b2,
	};

	if(colour_state)
		temp_cube=  create3DObject(GL_TRIANGLES, 36, g_vertex_buffer_data, g_color_buffer_data1, GL_FILL);
	else 
		temp_cube = create3DObject(GL_TRIANGLES, 36, g_vertex_buffer_data, g_color_buffer_data, GL_FILL);
	return temp_cube;
}


void draw ()
{


	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (programID);

	glm::vec3 eye ( 14*cos(camera_rotation_angle*M_PI/180.0f),14*sin(camera_rotation_angle*M_PI/180.0f),14*sin(camera_angle*M_PI/180.0));
	glm::vec3 target (0, 0, 0);
	glm::vec3 up (0, 0, 1);
	Matrices.view = glm::lookAt( eye, target, up );
	glm::mat4 VP = Matrices.projection * Matrices.view;
	glm::mat4 MVP;


	if(isjump && !isfall)
	{
		MAN_POS_Z+=MAN_VEL_Z;
		MAN_VEL_Z-=DECELERATION;
		if(MAN_POS_Z<=0.4)
		{
			isjump=0;
			MAN_VEL_Z=0.3;
			MAN_POS_Z=0.4;
		}
	}

	if(MAN_POS_Z<=0.4)
		isfall=true;


	if(isfall)
		MAN_POS_Z-=0.1;

	if(ismanrotate)
		man_angle+=man_rotatedirection*2;
	if(ismanincrement)
	{
		MAN_POS_X +=man_movedirection*.15*cos((man_angle+90)*M_PI/180.0);
		MAN_POS_Y +=man_movedirection*.15*sin((man_angle+90)*M_PI/180.0);
	}


	glm::mat4 rotateman = glm::rotate((float)(man_angle*M_PI/180.0f), glm::vec3(0,0,1)); 
	if(isincrementright)
		camera_rotation_angle+=1;
	if(isincrementleft)
		camera_rotation_angle-=1;

	if(isincrementdown)
		camera_angle-=1;
	if(isincrementup)
		camera_angle+=1;

















	int it=0;
	int it2=0;
	for(float i = 0 ; i < BOARDWIDTH ; i++)
		for(float j = 0 ; j < BOARDLENGTH ; j++){
			if(BOARD_MAP[(int)i][(int)j]){
				if(firsttime)
				{
					if(BOARD_MAP[(int)i][(int)j]==1){
					blockcenters.push_back(make_pair((i*1.6-1.6*BOARDWIDTH/2)*2,(j*1.6-1.6*BOARDLENGTH/2)*2));
					rotate_status.push_back(rand()&1);
					}
					else if(BOARD_MAP[(int)i][(int)j]==2){
						woodbrickcenters.push_back(make_pair((i*1.6-1.6*BOARDWIDTH/2)*2,(j*1.6-1.6*BOARDLENGTH/2)*2));
						brick_rotate_status.push_back(rand()&1);
					}
				}
				Matrices.model = glm::mat4(1.0f);
				if(BOARD_MAP[(int)i][(int)j]==1){
					glm::mat4 translateCube=glm::translate(glm::vec3((i*1.6-1.6*BOARDWIDTH/2)*2,(j*1.6-1.6*BOARDLENGTH/2)*2,0));
					if(rotate_status[it]){
						glm::mat4 rotatecube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(0,0,1));
						Matrices.model *= (translateCube)*rotatecube;
					}
					else
						Matrices.model *= (translateCube);
					MVP = VP * Matrices.model;
					glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
					draw3DObject(BLOCKS[0]);
					it++;
				}
				else if(BOARD_MAP[(int)i][(int)j]==2){
					glm::mat4 scaleCube=glm::scale(glm::vec3(0.9,0.3,0.5));
					glm::mat4 translateCube=glm::translate(glm::vec3((i*1.6-1.6*BOARDWIDTH/2)*2,(j*1.6-1.6*BOARDLENGTH/2)*2,0));
					glm :: mat4 positionCube=glm::translate(glm::vec3(0,-1.0,0));
					for(int l=1;l<=3;l++){
						if(brick_rotate_status[it2]){
							glm::mat4 rotatecube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(0,0,1));
							Matrices.model = (translateCube)*rotatecube*positionCube*scaleCube;
						}
						else
							Matrices.model = (translateCube)*positionCube*scaleCube;
						MVP = VP * Matrices.model;
						glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
						draw3DObject(WOODBRICK_1);
						positionCube=glm::translate(glm::vec3(0,-1+l,0));
					}
					it2++;
				}
			}
		}
	firsttime=false;

	check_fall();


	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 translatebody1=glm::translate(glm::vec3(0,0,3));
	glm:: mat4 tbody1=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *= tbody1*rotateman*(translatebody1);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(LOWER_BODY);

	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 translatebody2=glm::translate(glm::vec3(0,0,3.5));
	glm:: mat4 tbody2=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *= tbody2*rotateman*(translatebody2);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(UPPER_BODY);


	if(ismanincrement)
	{
		thigh_angle+=1*thigh_direction;
		leg_angle+=2.0/3.0*leg_direction;
	}

	if(leg_angle>=20)
		leg_direction=-1;
	if(leg_angle<=-20)
		leg_direction=1;


	
	if(thigh_angle>=30)
		thigh_direction=-1;
	if(thigh_angle<=-30)
		thigh_direction=1;


	for(int i=0;i<2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm:: mat4 Tleg1=glm::translate(glm::vec3(0,0,-1));
		glm::mat4 Rleg1 = glm::rotate((float)(2*(.5-i)*leg_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tleg2=glm::translate(glm::vec3(0,0,-1));
		glm::mat4 Rleg2 = glm::rotate((float)(2*(.5-i)*thigh_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tleg3=glm::translate(glm::vec3(-.3+i*.6,0,3));
		glm:: mat4 Tleg4=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
		Matrices.model *= Tleg4*rotateman*Tleg3*Rleg2*Tleg2*Rleg1*Tleg1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(LEGS[i]);
	}
	
	for(int i=0;i<2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm:: mat4 Tshoe1= glm::translate(glm::vec3(0,0,-1));
		glm::mat4 Rshoe1 = glm::rotate((float)(2*(.5-i)*leg_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tshoe2= glm::translate(glm::vec3(0,0,-1));
		glm::mat4 Rshoe2 = glm::rotate((float)(2*(.5-i)*thigh_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tshoe3= glm::translate(glm::vec3(-.3+i*.6,-0.3,3));
		glm:: mat4 Tshoe4= glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
		Matrices.model *=  Tshoe4*rotateman*Tshoe3*Rshoe2*Tshoe2*Rshoe1*Tshoe1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(SHOE[i]);
	}




	for(int i=0;i<2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm:: mat4 Tthigh1=glm::translate(glm::vec3(0,0,-1));
		glm::mat4 Rthigh1 = glm::rotate((float)(2*(.5-i)*thigh_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tthigh011=glm::translate(glm::vec3(-.3+i*0.6,0,3));
		glm:: mat4 Tthigh01=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
		Matrices.model *= Tthigh01*rotateman*Tthigh011*Rthigh1*Tthigh1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(THIGHS[i]);

	}

	for(int i=0;i<2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 Rshould1 = glm::rotate((float)(-2*(.5-i)*thigh_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Tshould1=glm::translate(glm::vec3(-0.8+i*1.6,0,4.5));
		glm:: mat4 Tshould4= glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
		Matrices.model *=  Tshould4*rotateman*Tshould1*Rshould1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(SHOULDER[i]);
	}


	for(int i=0;i<2;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm:: mat4 Thands1 = glm::translate(glm::vec3(0,0,-1));
		glm:: mat4 Rhands1 = glm::rotate((float)(-2*(.5-i)*leg_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Thands2 = glm::translate(glm::vec3(0,0,-0.8));
		glm:: mat4 Rhands2 = glm::rotate((float)(-2*(.5-i)*thigh_angle*M_PI/180.0f), glm::vec3(1,0,0));
		glm:: mat4 Thands3 = glm::translate(glm::vec3(-0.8+i*1.6,0,2.7+1.8));
		glm:: mat4 Thands4=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
		Matrices.model *= Thands4*rotateman*Thands3*Rhands2*Thands2*Rhands1*Thands1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(HANDS[i]);
	}


	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 Tneck=glm::translate(glm::vec3(0,0,4.5));
	glm:: mat4 Tneck01=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *= Tneck01*rotateman*(Tneck);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(NECK);

	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 Thead1=glm::translate(glm::vec3(0,0,4.7));
	glm:: mat4 Thead01=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *= Thead01*rotateman*(Thead1);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(LOWER_HEAD);

	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 Thead2=glm::translate(glm::vec3(0,0,4.9));
	glm:: mat4 Thead02=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *=Thead02*rotateman*(Thead2);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(MIDDLE_HEAD);

	Matrices.model = glm::mat4(1.0f);
	glm:: mat4 Thead3=glm::translate(glm::vec3(0,0,5.2));
	glm:: mat4 Thead03=glm::translate(glm::vec3(MAN_POS_X,MAN_POS_Y,MAN_POS_Z));
	Matrices.model *= Thead03*rotateman*(Thead3);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(UPPER_HEAD);

	


}

GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);
	glfwSetWindowCloseCallback(window, quit);
	glfwSetKeyCallback(window, keyboard);      
	glfwSetCharCallback(window, keyboardChar);  
	glfwSetMouseButtonCallback(window, mouseButton);
	return window;
}

void initGL (GLFWwindow* window, int width, int height)
{
//	for(int i=0;i<100;i++)

	cubeColour X;
	X.c1=205102029;X.c2=255165079;X.c3=205102029;X.c4=205102029;
	BLOCKS[0]=createCube(X,0);

	//BLUE STUFF
	X.c1=205;
	LOWER_BODY =createCube(X,1,-0.6,-0.4,0, 0.6,-0.4,0, -0.6,-0.4,0.5 ,0.6,-0.4,0.5, -0.6,0.4,0.5, 0.6,0.4,0.5, -0.6,0.4,0, 0.6,0.4,0);
	for(int i=0;i<2;i++)
		THIGHS[i]=createCube(X,1,-0.2,-0.4,0, 0.2,-0.4,0, -0.2-i*.1,-0.4,1.2 ,0.3,-0.4,1.2, -0.2-i*.1,0.4,1.2, 0.3,0.4,1.2, -0.2,0.4,0, 0.2,0.4,0);





	//SKIN STUFF
	X.c1=255160122;
	for(int i=0;i<2;i++)
		LEGS[i]=createCube(X,1,-0.2,-0.2,0, 0.2,-0.2,0, -0.2,-0.2,1.0 ,0.2,-0.2,1.0, -0.2,0.2,1.0, 0.2,0.2,1.0, -0.2,0.2,0 ,0.2,0.2,0);


	NECK=createCube(X,1, -0.2,-0.1,0, 0.2,-0.1,0, -0.1,-0.1,0.2 ,0.1,-0.1,.2, -0.1,0.1,.2, 0.1,0.1,.2, -0.2,0.1,0 ,0.2,0.1,0);

	MIDDLE_HEAD=createCube(X,1, -0.2,-0.2,0, 0.2,-0.2,0, -0.2,-0.2,0.3 ,0.2,-0.2,.3, -0.2,0.2,.3, 0.2,0.2,.3, -0.2,0.2,0 ,0.2,0.2,0);
	LOWER_HEAD=createCube(X,1,  -0.1,-0.1,0, 0.1,-0.1,0, -0.2,-0.2,0.2 ,0.2,-0.2,.2, -0.2,0.2,.2, 0.2,0.2,.2, -0.1,0.1,0 ,0.1,0.1,0);


	SHOULDER[0]=createCube(X,1,-0.2,-0.2,-0.8, 0.2,-0.2,-0.8, -0.1,-0.2,-0.2 ,0.2,-0.2,0, -0.1,0.2,-0.2, 0.2,0.2,0, -0.2,0.2,-0.8 ,0.2,0.2,-0.8);
	SHOULDER[1]=createCube(X,1,-0.2,-0.2,-0.8, 0.2,-0.2,-0.8, -0.2,-0.2,0 ,0.1,-0.2,-0.2, -0.2,0.2,0, 0.1,0.2,-0.2, -0.2,0.2,-0.8 ,0.2,0.2,-0.8);

	HANDS[0]=createCube(X,1,-0.1,-0.1,0, 0.1,-0.1,0, -0.15,-0.15,1.0 ,0.15,-0.15,1.0, -0.15,0.15,1.0, 0.15,0.15,1.0, -0.1,0.1,0 ,0.1,0.1,0);
	HANDS[1]=createCube(X,1,-0.1,-0.1,0, 0.1,-0.1,0, -0.15,-0.15,1.0 ,0.15,-0.15,1.0, -0.15,0.15,1.0, 0.15,0.15,1.0, -0.1,0.1,0 ,0.1,0.1,0);


	//BLACK STUFF
	X.c1=0;
	UPPER_BODY =createCube(X,1,-0.6,-0.4,0, 0.6,-0.4,0, -0.65,-0.4,1 ,0.65,-0.4,1, -0.65,0.4,1, 0.65,0.4,1, -0.6,0.4,0, 0.6,0.4,0);

	UPPER_HEAD=createCube(X,1,-0.2,-0.2,0, 0.2,-0.2,0, -0.1,-0.2,0.2 ,0.1,-0.2,.2, -0.1,0.2,.2, 0.1,0.2,.2, -0.2,0.2,0 ,0.2,0.2,0);
	
	for(int i=0;i<2;i++)
		SHOE[i]=createCube(X,1, -0.25,0,-0.4, 0.25,0,-0.4, -0.25,0,0 ,0.25,0,0, -0.25,.7,0, 0.25,0.7,0, -0.25,0.8,-0.4 ,0.25,0.8,-0.4);



	//WOOD COLOUR
	X.c1=107066038;X.c2=139090043;X.c3=X.c1;X.c4=X.c1;
	WOODBRICK_1=createCube(X,0);


	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	reshapeWindow (window, width, height);
	glClearColor (126.0/255.0,192.0/255.0,238.0/255.0, 1); // R, G, B, A
	glClearDepth (1.0f);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;
	GLFWwindow* window = initGLFW(width, height);
	initGL (window, width, height);
	double last_update_time = glfwGetTime(), current_time;
	while (!glfwWindowShouldClose(window)) 
	{
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
