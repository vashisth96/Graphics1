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

float camera_rotation_angle = 90;
struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;
int N = 20;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;

//*****************************//
////********* TANK ************//
//*****************************//

float TRANSLATE_TANK_BY = 0.0f;
float TANK_POS_X=0.0f;
float TURRET_ANGLE = 40;
float ROTATE_TURRET_BY = 0;
float GUN_ANGLE;
float BULLET_ANGLES[int(1e5)];
float BULLET_POS_X[int(1e5)];
float BULLET_POS_Y[int(1e5)];
double ENERGY_TIMER =0;
int BULLET_COUNT=0;
int BULLETS_GAP = 0;
int MAKE_TANK_BULLETS = 0;
int ENERGY_TIMER_LOCK = 0;
int DISPLAY_BULLETS[int(1e5)];
int showtankshield=0;
//****************************//
//********* BALL *************//
//****************************//

float VELOCITY = .5f;
float BALL_POS_X = TANK_POS_X;
float BALL_POS_Y = 0.0f;
float DECELERATION = 0.1f;
float BALL_VEL_X = 0.0f;
float BALL_VEL_Y = 0.0f;
int SHOW_BALL = 1;

//****************************//
//******* ENEMY TURRET *******//
//****************************//

float ENEMY_TURRET_POS_X =9.2f;
float ENEMY_TURRET_POS_Y =-9.0f;
float ENEMY_WALL_1_ANGLE = 10.0f;
int CHAINS_HIT_COUNT = 0;
int ENEMY_TURRET_HIT_COUNT = 0;
int SHOW_ENEMY_TURRET =1;
int SHOW_CHAINS = 1;
int CHAINS_HIT_LOCK = 0;
int ENEMY_TURRET_HIT_LOCK =0;
double CURSOR_X,CURSOR_Y;
int limit=0;
float enemybulletspos[10000];
int bulletcount=0;
int target_1;

//****************************//
//********** PLANE ***********//
//****************************//

float TRANSLATE_PLANE_BY = -.1;
float PLANE_POS_X = 12;
float PLANE_POS_Y = 0.0f;
int PLANE_HIT_COUNT = 0;

//////////blocks///////

float whiteblockfall=0;
float blackblockfall=0;
float greyblockfall=0;

float someconst=0;

float ortholeftx=-10.0f;
float orthorightx=10.f;
float orthoupy=10.0f;
float orthodowny=-10.0f;

float transplatx = 7.5f-1.0f;//-0.25f;
float transplaty = -4.0f;

float platformx = 7.5f-1.25f;
float platformy = -4.0f+0.15f;
int moved=0;
float blockfallangle=0;

float blackblockx = 4.8f;
float blackblocky= -3.2f;
float greyblockx = 5.0f;
float greyblocky = -2.2f;
float whiteblockx =4.8f;
float whiteblocky = -1.2f;

int showfloor=1;

int RESPAWN_TURRET =0;

int floorlock=1;
int floorhitcount=0;
int message=1;
int side=0;
float blockfallangle2=0;
int message2=1;
int side2=0;
int moved2=0;
int hitback=0;

int wallcounter=0;

float camera_up=0;
float camera_down=0;
float camera_left=0;
float camera_right=0;
float camera_zoom=0;
float camera_zoomout=0;


float redbar[100];
float greenbar[100];
float bluebar[100];

int hitenemybullets[100000];

int fallstop1=1;
int fallstop2=1;
int fallstop3=1;

int alldown=0;
int alldownlast=0;
int alldowncount=0;


int blackboxlimit=0;
int whiteboxlimit=0;
int greyboxlimit=0;

float enemystandwallangle=0.0f;
float enemystandwallangleadd=0.0f;

int showblackbox=1;
int showgreybox=1;
int showwhitebox=1;

vector<int> sevensegdecoder[10];
int score=0;


float planebulletsx[100];
float planebulletsy[100];
int planebulletlimit=1;
float planebulletacc[100];


int quitgame=0;
int healthlimit=16;


VAO *GROUND,*SKY,*WHEELS[20],*TANK_BODY_UPPER,*TANK_BODY_LOWER,*TANK_TURRET_BASE,*TANK_SPROCKET_LEFT;
VAO *TANK_SPROCKET_RIGHT,*TANK_TRACK,*TANK_TURRET,*TANK_TURRET_END,*BALL,*ENEMY_WALL_1,*CHAINS[20];
VAO *ENEMY_TURRET_BASE,*ENEMY_TURRET_RING,*ENEMY_TURRET_BODY,*ENEMY_TURRET_GUN,*ENEMY_PLATFORM,*ENEMY_PLATFORM_WALL;
VAO *DUMMYBOX,*SMOKE_LOWER,*SMOKE_UPPER,*TANK_GUN,*TANK_GUN_BASE,*TANK_GUN_HANDLE;
VAO *PLANE_NOSE,*PLANE_BODY_1,*PLANE_BODY_2,*PLANE_BODY_3,*PLANE_TAIL,*PLANE_WING,*PLANE_COCKPIT,*TANK_BULLETS[100000];
VAO *TANK_GUN_BARREL,*TANK_GUN_TRIGGER,*enemybullets[100000],*tankshield,*ENEMY_PLATFORM_WALL2,*ENEMY_PLATFORM2,*ENEMY_PLATFORM3;
VAO * ENEMY_PLATFORM_WALL3,*ENEMY_STAND_WALL,*enemywallhinge1,*enemywallhinge2,*energybar,*energybaroutline,*energyliquid;
VAO * healthbars[100],*healthbarback,*healthbaroutline,*sevenseg[15],*planebullets[10000];
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
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors
	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

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
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices);
}

float triangle_rot_dir = 5;
float rectangle_rot_dir = 5;
bool triangle_rot_status = false;
bool rectangle_rot_status = false;

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) {
		switch (key) 
		{
			case GLFW_KEY_W:
				ROTATE_TURRET_BY=0;
				break;
			case GLFW_KEY_S:
				ROTATE_TURRET_BY=0;
				break;
			case GLFW_KEY_A:
				TRANSLATE_TANK_BY = 0.0f;
				break;
			case GLFW_KEY_D:
				TRANSLATE_TANK_BY = 0.0f; 
				break;
			case GLFW_KEY_SPACE:
				floorlock=1;
				ENERGY_TIMER_LOCK=0;
				BALL_POS_Y = 0.0f;
				BALL_POS_X = TANK_POS_X;
				BALL_VEL_X = VELOCITY *ENERGY_TIMER/40.0 * cos( TURRET_ANGLE * M_PI/180.0);
				BALL_VEL_Y = VELOCITY *ENERGY_TIMER/40.0* sin( TURRET_ANGLE * M_PI/180.0);
				DECELERATION = BALL_VEL_Y/20.0;
				SHOW_BALL = 1;
				ENEMY_TURRET_HIT_LOCK=CHAINS_HIT_LOCK=1;
				ENERGY_TIMER=0;
				break;
			case GLFW_KEY_UP:
				camera_up=0;
				break;
			case GLFW_KEY_DOWN:
				camera_down=0;
				break;
			case GLFW_KEY_LEFT:
				camera_left=0;
				break;
			case GLFW_KEY_RIGHT:
				camera_right=0;
				break;
			case GLFW_KEY_Z:
				camera_zoom=0;
				break;
			case GLFW_KEY_X:
				camera_zoomout=0;
				break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) 
		{
			case GLFW_KEY_W:
				ROTATE_TURRET_BY += 1.0f; 
				break;
			case GLFW_KEY_S:
				ROTATE_TURRET_BY += -1;
				break;
			case GLFW_KEY_A:
				TRANSLATE_TANK_BY = -0.05f;
				break;
			case GLFW_KEY_D:
				TRANSLATE_TANK_BY = 0.05f;
				break;
			case GLFW_KEY_SPACE:
				ENERGY_TIMER=0;
				ENERGY_TIMER_LOCK=1;
				hitback=0;
				break;
			case GLFW_KEY_UP:
				camera_up=1;
				orthodowny+=0.1f;
				orthoupy+=0.1f;
				break;
			case GLFW_KEY_DOWN:
				camera_down=1;
				orthodowny-=0.1f;
				orthoupy-=0.1f;
				break;
			case GLFW_KEY_LEFT:
				camera_left=1;
				ortholeftx-=0.1;
				orthorightx-=0.1;
				break;
			case GLFW_KEY_RIGHT:
				camera_right=1;
				ortholeftx+=0.5f;
				orthorightx+=0.5f;
				break;
			case GLFW_KEY_Z:
				camera_zoom=1;
				orthoupy-=0.1f;
				orthodowny+=0.1f;
				ortholeftx+=0.1f;
				orthorightx-=0.1f;
				break;
			case GLFW_KEY_X:
				camera_zoomout=1;
				orthoupy+=0.1f;
				orthodowny-=0.1f;
				ortholeftx-=0.1f;
				orthorightx+=0.1f;
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
			if(action == GLFW_PRESS)
			{
				MAKE_TANK_BULLETS=1;
			}
			else if (action == GLFW_RELEASE)
			{
				MAKE_TANK_BULLETS=0;
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_RELEASE) 
			{
				showtankshield=0;
			}
			if(action==GLFW_PRESS)
			{
				showtankshield=1;

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
	Matrices.projection = glm::ortho(ortholeftx, orthorightx ,orthodowny, orthoupy, 0.1f, 500.0f);
}



bool intersects(float platform_x,float platform_y,float rectwidth,float rectheight,float circler)
{
	float circleDistancex = abs(BALL_POS_X-6.7f - platform_x);//(circle.x - rect.x);
	float circleDistancey = abs(BALL_POS_Y-7.0f - platform_y);

	if (circleDistancex > (rectwidth/2.0f + circler)) { return false; }
	if (circleDistancey > (rectheight/2.0f + circler)) { return false; }

	if (circleDistancex <= (rectwidth/2.0f)) { return true; } 
	if (circleDistancey <= (rectheight/2.0f)) { return true; }

	float cornerDistance_sq = pow((circleDistancex - rectwidth/2.0f),2) +
		pow((circleDistancey - rectheight/2.0f),2);

	return (cornerDistance_sq <= (pow(circler,2)));
}

VAO * createCircle( GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides )
{
	int numberOfVertices = numberOfSides + 2,i;
	GLfloat twicePi = 2.0f * M_PI;

	GLfloat circleVerticesX[numberOfVertices];
	GLfloat circleVerticesY[numberOfVertices];
	GLfloat circleVerticesZ[numberOfVertices];

	circleVerticesX[0] = x;
	circleVerticesY[0] = y;
	circleVerticesZ[0] = z;

	for ( i = 1; i < numberOfVertices; i++ )
	{
		circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
		circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
		circleVerticesZ[i] = z;
	}

	GLfloat allCircleVertices[( numberOfVertices ) * 3];
	GLfloat allCircleColors[( numberOfVertices ) * 3 * 3];

	for ( int i = 0; i < numberOfVertices; i++ )
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
		allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
	}
	for (int j=0;j<330;j++)
	{
		allCircleColors[j]=.517f;
	}
	VAO * tempcircle = create3DObject(GL_TRIANGLE_FAN,110, allCircleVertices, allCircleColors, GL_TRIANGLES);
	return tempcircle;
}

VAO * createTriangle (float r1,float g1,float b1,float r2,float g2,float b2,float x1,float y1,float x2,float y2,float x3,float y3)
{
	VAO *triangle;
	GLfloat vertex_buffer_data [] = {x1,y1,0 , x2,y2,0 , x3,y3,0};
	GLfloat color_buffer_data [] = {r1,g1,b1, r2,g2,b2, r2,g2,b2,};
	triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_FILL);
	return triangle;
}

VAO * createRectangle (float r1,float g1,float b1,float r2,float g2,float b2,float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4)
{
	VAO * rectangle;
	GLfloat vertex_buffer_data [] = { x1,y1,0, x2,y2,0 , x3,y3,0, x4,y4,0, x3,y3,0 , x2,y2,0};
	GLfloat color_buffer_data [] = {  r1,g1,b1, r1,g1,b1, r2,g2,b2,  r2,g2,b2, r2,g2,b2, r1,g1,b1}; // 1 , 2 , 3 , 4 , 3, 2 
	rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	return rectangle;
}

VAO * createRectangleOutline (float r1,float g1,float b1,float r2,float g2,float b2,float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4)
{
	VAO * rectangle;
	GLfloat vertex_buffer_data [] = { x1,y1,0, x2,y2,0 , x3,y3,0, x4,y4,0, x3,y3,0 , x2,y2,0};
	GLfloat color_buffer_data [] = {  r1,g1,b1, r1,g1,b1, r2,g2,b2,  r2,g2,b2, r2,g2,b2, r1,g1,b1}; // 1 , 2 , 3 , 4 , 3, 2 
	rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_LINE);
	return rectangle;
}

void draw ()
{



	energyliquid = createRectangle(.0,1.0,.0,.0,1.0,.0, 0.1f,0.2f, 0.9f,0.2f , 0.1f,max(0.2,7.8*ENERGY_TIMER/170.0), 0.9f,max(0.2,7.4*ENERGY_TIMER/170.0));

	if(blackblocky -0.01 <=whiteblocky && blackblocky+0.01>=whiteblocky && greyblocky -0.01 <=whiteblocky && greyblocky+0.01>=whiteblocky)
		alldown=1;
	if(alldown)
		alldowncount++;
	if(alldowncount>=200)
		alldownlast=1;
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (programID);
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	glm::vec3 target (0, 0, 0);
	glm::vec3 up (0, 1, 0);
	Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 VP = Matrices.projection * Matrices.view;
	glm::mat4 MVP;	


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatesky = glm::translate (glm::vec3(0.0f, 0.0f, 0.0f)); 
	Matrices.model*=translatesky;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(SKY);

	if(showtankshield)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 ptankshield = glm::translate (glm::vec3(-7.5f+TANK_POS_X,-9.0f, 0.0f)); 
		Matrices.model*= ptankshield;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(tankshield);
	}

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translateRectangle = glm::translate (glm::vec3(0.0f, -1.0f, 0.0f)); 
	Matrices.model*=translateRectangle;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(GROUND);


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatetankoutline1 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=translatetankoutline1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_TRACK);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 rotatetankturret = glm::rotate((float)(TURRET_ANGLE*M_PI/180.0f), glm::vec3(0,0,1));
	glm::mat4 translatetankturret = glm::translate (glm::vec3(TANK_POS_X-7.2f, -7.3f-1.0f, 0.0f)); 
	Matrices.model*=translatetankturret*rotatetankturret;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_TURRET);


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatetankturretend = glm::translate (glm::vec3(TANK_POS_X-7.2f, -7.3f-1.0f, 0.0f)); 
	Matrices.model*=translatetankturretend*rotatetankturret;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]); 
	draw3DObject(TANK_TURRET_END);



	TURRET_ANGLE += ROTATE_TURRET_BY;
	if(TURRET_ANGLE >= 90)
		TURRET_ANGLE = 90.0f;
	if(TURRET_ANGLE <= 20)
		TURRET_ANGLE = 20.0f;

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatetank3 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=translatetank3;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_TURRET_BASE);

	for(int i=0;i<5;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translatecircle = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
		Matrices.model*=translatecircle;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(WHEELS[i]);
	}

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tendcircle1 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=tendcircle1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_SPROCKET_LEFT);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tendcircle2 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=tendcircle2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_SPROCKET_RIGHT);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatetank1 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=translatetank1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_BODY_UPPER);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translatetank2 = glm::translate (glm::vec3(TANK_POS_X, -1.0f, 0.0f)); 
	Matrices.model*=translatetank2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_BODY_LOWER);


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tankgunbase1 = glm::translate (glm::vec3(TANK_POS_X-8.3f, -8.3f, 0.0f)); 
	Matrices.model*=tankgunbase1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_GUN_BASE);

	TANK_POS_X += TRANSLATE_TANK_BY;

	if(SHOW_BALL)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translateball = glm::translate (glm::vec3(BALL_POS_X-7.3f, BALL_POS_Y-7.3f-1.0f, 0.0f)); 
		Matrices.model*=translateball*rotatetankturret;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(BALL);
	}

	if(!SHOW_ENEMY_TURRET || RESPAWN_TURRET)
	{
		if(TANK_POS_X>11.2f)
			TANK_POS_X=11.2f;
	if(BALL_POS_X-6.9f>=3.5f && BALL_POS_X-6.9f<=4.5 & BALL_POS_Y-7.0<=-3.5f)
		{
			BALL_VEL_X*=-1;
			hitback=1;
			cout<<"This one"<<endl;
		}
	}
	if(SHOW_BALL)
	{

		BALL_POS_X += BALL_VEL_X;
		BALL_POS_Y += BALL_VEL_Y;
		BALL_VEL_Y-=DECELERATION;
	}
	if(BALL_POS_Y<=-2.0)
	{
		BALL_VEL_Y*=-0.6;
		BALL_VEL_X-=0.005;
	}
	if(BALL_VEL_X<=0 && !hitback)
	{
		BALL_VEL_X=0.0f;
		BALL_VEL_Y=0.0f;
		DECELERATION=0.0f;
		SHOW_BALL=0;
	}
	if(hitback)
	{
		BALL_VEL_X+=0.005;
		if(BALL_VEL_X>=0)
		{
			BALL_VEL_X=0.0f;
			BALL_VEL_Y=0.0f;
			DECELERATION=0.0f;
			SHOW_BALL=0;
		}
	}

	if(CHAINS_HIT_LOCK && BALL_POS_X-6.9f >=9.0f && BALL_POS_X -6.9f <= 10.0f && BALL_POS_Y <= 0.2f && BALL_POS_Y >=-2)
	{
		CHAINS_HIT_COUNT++;
		CHAINS_HIT_LOCK=0;
	}
	if(!SHOW_CHAINS && ENEMY_TURRET_HIT_LOCK && BALL_POS_X -6.9 >= 8.4f && BALL_POS_X-6.9 <=10.0f && BALL_POS_Y <= -1.1f && BALL_POS_Y >= -9.0f)
	{
		score+=5;
		ENEMY_TURRET_HIT_COUNT++;
		ENEMY_TURRET_HIT_LOCK=0;
	}
	if(ENEMY_TURRET_HIT_COUNT>=4)
		SHOW_ENEMY_TURRET = 0;

	if(CHAINS_HIT_COUNT>=3)
		SHOW_CHAINS=0;

	if(!SHOW_CHAINS)
	{
		ENEMY_WALL_1_ANGLE+=4;
	}
	if(ENEMY_WALL_1_ANGLE>=90.0f)
		ENEMY_WALL_1_ANGLE = 90.0f;

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 rotateenemywall1 = glm::rotate((float)(ENEMY_WALL_1_ANGLE*M_PI/180.0f), glm::vec3(0,0,1));
	glm::mat4 tenemywall1 = glm::translate (glm::vec3(8.0, -9.0f, 0.0f)); 
	Matrices.model*=tenemywall1*rotateenemywall1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_WALL_1);

	if(SHOW_CHAINS)
	{
		for(int i=0;i<13;i++)
		{
			Matrices.model = glm::mat4(1.0f);
			glm::mat4 translatechains = glm::translate (glm::vec3(7.7, -7.1f, 0.0f)); 
			Matrices.model*=translatechains;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(CHAINS[i]);
		}
	}

	if(SHOW_ENEMY_TURRET || RESPAWN_TURRET)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 t4 = glm::translate (glm::vec3(ENEMY_TURRET_POS_X, ENEMY_TURRET_POS_Y, 0.0f)); 
		Matrices.model*=t4;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_TURRET_GUN);

		Matrices.model = glm::mat4(1.0f);
		glm::mat4 t1 = glm::translate (glm::vec3(ENEMY_TURRET_POS_X, ENEMY_TURRET_POS_Y, 0.0f));
		Matrices.model*=t1;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_TURRET_BASE);

		Matrices.model = glm::mat4(1.0f);
		glm::mat4 t2 = glm::translate (glm::vec3(ENEMY_TURRET_POS_X, ENEMY_TURRET_POS_Y,0.0f)); 
		Matrices.model*=t2;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_TURRET_RING);

		Matrices.model = glm::mat4(1.0f);
		glm::mat4 t3 = glm::translate (glm::vec3(ENEMY_TURRET_POS_X, ENEMY_TURRET_POS_Y, 0.0f)); 
		Matrices.model*=t3;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_TURRET_BODY);
	}

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tplat1 = glm::translate (glm::vec3(transplatx, transplaty, 0.0f)); 
	Matrices.model*=tplat1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_PLATFORM);


	if(showfloor && floorlock && BALL_POS_X-6.7f<=platformx+2.0f && BALL_POS_X-6.7f >=platformx+0.2 && BALL_POS_Y-7.0f <=platformy +0.8f && BALL_POS_Y-7.0f >=platformy-0.8f)
	{
		floorhitcount++;
		floorlock=0;
	}

	if(floorhitcount==5)
		showfloor=0;

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tplat01 = glm::translate (glm::vec3(transplatx, transplaty, 0.0f)); 
	Matrices.model*=tplat01;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_PLATFORM2);

	if(showfloor)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 tplat02 = glm::translate (glm::vec3(transplatx, transplaty, 0.0f)); 
		Matrices.model*=tplat02;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_PLATFORM3);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////// 


	if(floorlock && BALL_POS_X-6.7f<=platformx+2.0f && BALL_POS_X-6.7f >=platformx+0.2 && BALL_POS_Y-7.0f <=platformy +0.8f && BALL_POS_Y-7.0f >=platformy-0.8f);
	else if(intersects(platformx,platformy,5.5f,0.8f,0.2f))
	{
		BALL_VEL_Y*=-1.0;
		BALL_VEL_X-=0.005;
	}

	if(intersects(greyblockx,greyblocky,1.0f,1.0f,0.2f))
	{
		hitback=1;
		greyblockx+=0.20f;
	}

	if(intersects(blackblockx,blackblocky,1.0f,1.0f,0.2f))
	{
		hitback=1;
		blackblockx+=0.20f;
	}
	if(intersects(whiteblockx,whiteblocky,1.0f,1.0f,0.2f))
	{

		hitback=1;
		whiteblockx+=0.20f;
	}
	if(hitback && BALL_VEL_X>0)
		BALL_VEL_X*=-1.0;


	///////////////////////////////////////////////////////////////////////////////////////////////

	if(!alldownlast && fabs(blackblockx-greyblockx)>0.5f /* && message*/ && fabs(blackblocky-greyblocky)<=1.1f && fabs(blackblocky-greyblocky)>0.9f)
	{
		moved=1;
		if(blackblockx<greyblockx)
			side=1;
		else
			side=-1;
	}
	if(!alldownlast && fabs(whiteblockx-greyblockx)>0.5f /* && message2 */&& fabs(whiteblocky-greyblocky)<=1.1f && fabs(whiteblocky-greyblocky)>=0.9f)
	{
		moved2=1;
		if(whiteblockx>greyblockx)
			side2=1;
		else
			side2=-1;
	}
	if(!alldownlast && fabs(whiteblockx-blackblockx)>0.5 /*&& message2*/ && fabs(whiteblocky-blackblocky)<=1.1f && fabs(whiteblocky-blackblocky)>=0.9f)

	{
		moved2=1;
		if(whiteblockx>blackblockx)
			side2=1;
		else
			side2=-1;

	}

	if(!alldownlast && fabs(whiteblocky-greyblocky)>1.1)
	{
		side2=0;
		moved2=1;
	}
	if(fabs(whiteblockx-blackblockx)<1.1 && fabs(whiteblocky-blackblocky)<0.1)
	{
		if(whiteblockx<blackblockx)
			blackblockx=whiteblockx+1.0f;
		else
			whiteblockx=blackblockx+1.0f;////////////////////////////////////////
	}
	if(fabs(whiteblockx-greyblockx)<1.1 && fabs(whiteblocky-greyblocky)<0.1)
	{
		if(whiteblockx<greyblockx)
			greyblockx=whiteblockx+1.0f;////////////////////////////////////////
		else
			whiteblockx=greyblockx+1.0f;
	}
	if(fabs(blackblockx-greyblockx)<1.1 && fabs(blackblocky-greyblocky)<0.1)
	{
		if(greyblockx<blackblockx)
			blackblockx=greyblockx+1.0f;///////////////////////////////////////
		else
			greyblockx=blackblockx+1.0f;
	}

	////savers//////////////////
	if(!alldownlast && whiteblocky-0.7f<=platformy)
	{
		moved2=0;
		//message2=0;
		whiteblocky=blackblocky;
	}
	if(!alldownlast && greyblocky-0.7f<=platformy)
	{
		moved=0;
		// message=0;
		greyblocky=blackblocky;
	}

	if(whiteblockx>platformx+0.6 && fallstop1 &&!showfloor)
	{
		whiteblockfall-=0.1f;
		whiteblocky+=whiteblockfall;
	}
	else if(blackblockx>platformx+0.6 && fallstop2 &&!showfloor)
	{
		blackblockfall-=0.1f;
		blackblocky+=blackblockfall;
	}
	else if(greyblockx>platformx+0.6 && fallstop3 && !showfloor)
	{
		greyblockfall-=0.1f;
		greyblocky+=greyblockfall;
	}
	if(whiteblocky<=-8.5f)
	{
		fallstop1=0;
		whiteblocky=-8.5f;
	}
	if(blackblocky<=-8.5f)
	{
		blackblocky=-8.5f;
		fallstop2=0;
	}
	if(greyblocky<=-8.5f)
	{
		greyblocky=-8.5f;
		fallstop3=0;
	}
	if(!fallstop1)
	{
		whiteblocky=-8.5f;
		whiteboxlimit++;
	}
	if(!fallstop2)
	{
		blackblocky=-8.5f;
		blackboxlimit++;
	}
	if(!fallstop3)
	{
		greyblocky=-8.5f;
		greyboxlimit++;
	}

	if(whiteboxlimit>=200)
		showwhitebox=0;
	if(blackboxlimit>=200)
		showblackbox=0;
	if(greyboxlimit>=200)
		showgreybox=0;

	if(showblackbox)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 tplat2 = glm::translate (glm::vec3(blackblockx,blackblocky,0.0f)); 
		Matrices.model*=tplat2;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(ENEMY_PLATFORM_WALL);
	}

	if(showwhitebox)
	{
		Matrices.model = glm::mat4(1.0f);
	glm::mat4 twhitewall = glm::translate (glm::vec3(whiteblockx,whiteblocky,0.0f)); 
	if(moved2)
	{
		whiteblocky-=0.1;
		blockfallangle2-=1;
		whiteblockx=whiteblockx +side2*.05;
		glm::mat4 rplatform3 = glm::rotate((float)(blockfallangle2*M_PI/180.0f), glm::vec3(0,0,1));
		Matrices.model*=twhitewall*rplatform3;
	}
	else
		Matrices.model*=twhitewall;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_PLATFORM_WALL3);

	}
	if(showgreybox)
	{
	Matrices.model = glm::mat4(1.0f);
	glm::mat4 tplat3 = glm::translate (glm::vec3(greyblockx,greyblocky,0.0f)); 
	if(moved)
	{
		greyblocky-=0.1;
		blockfallangle-=2;
		greyblockx=greyblockx +side*0.05;
		glm::mat4 rplatform2 = glm::rotate((float)(blockfallangle*M_PI/180.0f), glm::vec3(0,0,1));
		Matrices.model*=tplat3*rplatform2;
	}
	else
		Matrices.model*=tplat3;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_PLATFORM_WALL2);

	}

	if(ENERGY_TIMER_LOCK)
		ENERGY_TIMER+=4;
	if(ENERGY_TIMER>=170)
		ENERGY_TIMER=0;

	PLANE_POS_X+=TRANSLATE_PLANE_BY;
	if(PLANE_POS_X<=-20)
	{
		PLANE_POS_X=12;
		PLANE_POS_Y=float((rand()%15)-5);
		PLANE_HIT_COUNT=0;
		planebulletlimit=rand()%5;
		for(int i=0;i<=planebulletlimit;i++)
		{
			planebulletsx[i]=(rand()%16)-8.0f;
			planebulletsy[i]=PLANE_POS_Y-0.5;
			planebulletacc[i]=0;
		}
	}

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pcockpit = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*=pcockpit;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_COCKPIT);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pnose = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*=pnose;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_NOSE);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pbody1 = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*=pbody1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_BODY_1);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pbody2 = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*=pbody2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_BODY_2);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pbody3 = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*=pbody3;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_BODY_3);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 ptail = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*= ptail;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_TAIL);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 pwing = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*= pwing;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(PLANE_WING);

	if(BALL_POS_X-6.9>=PLANE_POS_X-0.5 && BALL_POS_X-6.9<=PLANE_POS_X+1.5 && BALL_POS_Y-7.9f <= PLANE_POS_Y +1.0 && BALL_POS_Y- 7.9f >=PLANE_POS_Y-1.0)
	{
		PLANE_POS_Y=float((rand()%15)-5);
		PLANE_POS_X=12;
		PLANE_HIT_COUNT=0;
		score+=1;
	}

	/* Matrices.model = glm::mat4(1.0f);                                   
	   glm::mat4 dummy1 = glm::translate (glm::vec3(BALL_POS_X-6.9f, BALL_POS_Y-7.0f, 0.0f)); 
	   Matrices.model*= dummy1;  // -0.5; +1.5 
	   MVP = VP * Matrices.model; //+0.5 -0.3 
	   glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	   draw3DObject(DUMMYBOX);
	   */
	//(BALL_POS_X-7.3f, BALL_POS_Y-7.3f-1.0f, 0.0f)  Matrices.model = glm::mat4(1.0f);




	Matrices.model = glm::mat4(1.0f);                                   
	glm::mat4 psmoke1 = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*= psmoke1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(SMOKE_UPPER);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 psmoke2 = glm::translate (glm::vec3(PLANE_POS_X, PLANE_POS_Y, 0.0f)); 
	Matrices.model*= psmoke2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(SMOKE_LOWER);

	if(MAKE_TANK_BULLETS)
	{
		BULLETS_GAP++;
		if(BULLETS_GAP>=5)
		{
			BULLET_ANGLES[BULLET_COUNT]=GUN_ANGLE;
			BULLET_POS_X[BULLET_COUNT]=TANK_POS_X-9.6f+1.5f*cos(GUN_ANGLE);
			BULLET_POS_Y[BULLET_COUNT]=-7.7f+1.5f*sin(GUN_ANGLE);
			BULLET_COUNT++;
			BULLETS_GAP=0;
		}
	}

	for(int i=0;i<BULLET_COUNT;i++)
	{
		Matrices.model = glm::mat4(1.0f);

		if(!DISPLAY_BULLETS[i])
		{
			BULLET_POS_X[i]+=.2f*cos(BULLET_ANGLES[i]);
			BULLET_POS_Y[i]+=.2f*sin(BULLET_ANGLES[i]);
			if(BULLET_POS_X[i]+1.5>=PLANE_POS_X-0.5f && BULLET_POS_X[i]+1.5<=PLANE_POS_X+1.5f && BULLET_POS_Y[i]<=PLANE_POS_Y+0.5 && BULLET_POS_Y[i]>=PLANE_POS_Y-0.5f)
			{
				PLANE_HIT_COUNT++;
				DISPLAY_BULLETS[i]=1;
			}
			if(PLANE_HIT_COUNT>=7)
			{
				PLANE_POS_Y=float((rand()%15)-5);
				PLANE_POS_X=12;
				PLANE_HIT_COUNT=0;
				score+=2;
			}

			glm::mat4 tankbull1 = glm::translate (glm::vec3(BULLET_POS_X[i], BULLET_POS_Y[i], 0.0f));
			Matrices.model*=tankbull1;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(TANK_BULLETS[i]);
		}
	}

	float len_x = CURSOR_X+10.0f-TANK_POS_X-1.0f;
	float len_y = 6+CURSOR_Y;
	GUN_ANGLE = atan2(len_y,len_x);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 ptankgun = glm::translate (glm::vec3(TANK_POS_X-8.2f, -7.7f, 0.0f)); 
	glm::mat4 rtankgun = glm::rotate((float)(GUN_ANGLE), glm::vec3(0,0,1));
	Matrices.model*= ptankgun*rtankgun;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_GUN);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 ptankgunhandle = glm::translate (glm::vec3(TANK_POS_X-8.2f, -7.7f, 0.0f)); 
	glm::mat4 rtankgunhandle = glm::rotate((float)(GUN_ANGLE), glm::vec3(0,0,1));
	Matrices.model*= ptankgunhandle*rtankgunhandle;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_GUN_HANDLE);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 ptankgunend = glm::translate (glm::vec3(TANK_POS_X-8.2f, -7.7f, 0.0f)); 
	glm::mat4 rtankgunend = glm::rotate((float)(GUN_ANGLE), glm::vec3(0,0,1));
	Matrices.model*= ptankgunend*rtankgunend;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_GUN_BARREL);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 ptankgunend2 = glm::translate (glm::vec3(TANK_POS_X-8.2f, -7.7f, 0.0f)); 
	glm::mat4 rtankgunend2 = glm::rotate((float)(GUN_ANGLE), glm::vec3(0,0,1));
	Matrices.model*= ptankgunend2*rtankgunend2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(TANK_GUN_TRIGGER);


	if(!SHOW_CHAINS && SHOW_ENEMY_TURRET || RESPAWN_TURRET)
	{
		if(!limit)
			target_1=rand()%200+1;
		limit++;
		if(limit==target_1)
		{
			bulletcount++;
			limit=0;
		}

		for(int i=0;i<bulletcount;i++)
		{
			enemybulletspos[i]-=0.5;
			if(enemybulletspos[i]+6.0f-showtankshield*1.25f<=-(10-(1+TANK_POS_X)))
			{
				if(!showtankshield && !hitenemybullets[i])
				{
					hitenemybullets[i]=1;
					healthlimit--;
				}
				if(showtankshield)
					hitenemybullets[i]=1;
				continue;
			}
			if(((whiteboxlimit>0 && whiteboxlimit<200) ||(blackboxlimit>0 && blackboxlimit<200)|| (greyboxlimit>0 && greyboxlimit<200 ) )&& enemybulletspos[i]+8.0f<6.0f)
					continue;
			Matrices.model = glm::mat4(1.0f);
			glm::mat4 penemybullets = glm::translate (glm::vec3(8.0f+enemybulletspos[i],-8.3f, 0.0f)); 
			Matrices.model*= penemybullets;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(enemybullets[i]);
		}
	}
	
	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penemystandwall = glm::translate (glm::vec3(5.0f,-4.0f, 0.0f)); 
	if(!SHOW_ENEMY_TURRET && wallcounter<=400)
			wallcounter++;

	if(wallcounter>=300)
		RESPAWN_TURRET=1;
	if(!SHOW_ENEMY_TURRET && wallcounter>=100 || RESPAWN_TURRET)
	{
		glm::mat4 renemystandwall = glm::rotate((float)(enemystandwallangle*M_PI/180.0), glm::vec3(0,0,1));
		Matrices.model*= penemystandwall*renemystandwall;
	}
	else
		Matrices.model*= penemystandwall;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(ENEMY_STAND_WALL);

	if(!SHOW_ENEMY_TURRET)
	{
		if(enemystandwallangle<=-90.0f)
			enemystandwallangle=-90.0f;
		else if(wallcounter>=100)
		{
			enemystandwallangleadd+=.1;
			enemystandwallangle-=enemystandwallangleadd;
		}
	}

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penemyhinge1 = glm::translate (glm::vec3(5.2f,-3.9f, 0.0f)); 
	Matrices.model*= penemyhinge1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(enemywallhinge1);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penemyhinge2 = glm::translate (glm::vec3(5.2f,-3.8f, 0.0f)); 
	Matrices.model*= penemyhinge2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(enemywallhinge2);




	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penergybarout = glm::translate (glm::vec3(-9.5,0, 0.0f)); 
	Matrices.model*= penergybarout;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(energybaroutline);



	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penergybar = glm::translate (glm::vec3(-9.5,0, 0.0f)); 
	Matrices.model*= penergybar;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(energybar);


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 penergybarliq = glm::translate (glm::vec3(-9.5,0, 0.0f)); 
	Matrices.model*= penergybarliq;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(energyliquid);


	Matrices.model = glm::mat4(1.0f);
	glm::mat4 phealth1 = glm::translate (glm::vec3(-8.4,8, 0.0f)); 
	Matrices.model*= phealth1;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(healthbaroutline);

	Matrices.model = glm::mat4(1.0f);
	glm::mat4 phealth2 = glm::translate (glm::vec3(-8.4,8, 0.0f)); 
	Matrices.model*= phealth2;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(healthbarback);

	
	float lenbar=0;
	for(int i=0;i<21;i++)
	{
		healthbars[i]=createRectangle(redbar[healthlimit]/255.0,greenbar[healthlimit]/255.0,bluebar[healthlimit]/255.0 ,redbar[healthlimit]/255.0,greenbar[healthlimit]/255.0,bluebar[healthlimit]/255.0 ,.4f+lenbar,0.2f, 0.65f+lenbar,0.2f , 0.4f+lenbar,1.0f, 0.65f+lenbar,1.0f);
		lenbar+=0.35;
	}

	
	for(int i=0;i<=healthlimit;i++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 phealth3 = glm::translate (glm::vec3(-8.5,8, 0.0f)); 
		Matrices.model*= phealth3;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(healthbars[i]);
	}
	if(healthlimit<0)
		quitgame=1;



	int temp=score;
	float scorex=0.0f;
	while(1)
	{

		for(int i=0;i<sevensegdecoder[temp%10].size();i++)
		{
			Matrices.model = glm::mat4(1.0f);
			glm::mat4 pscore = glm::translate (glm::vec3(7.5f+scorex,7.5f, 0.0f)); 
			Matrices.model*= pscore;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(sevenseg[sevensegdecoder[temp%10][i]]);
		}
		temp/=10;
		if(!temp)
			break;
		scorex-=1;
	}

	for(int i=0;i<=planebulletlimit;i++)
	{
		if(PLANE_POS_X+0.4f<=planebulletsx[i])
		{
			if(planebulletsy[i]<=-15)
				planebulletsx[i]=-20;
			if(planebulletsy[i]<=-8.0f && TANK_POS_X-8.0f<=planebulletsx[i] && TANK_POS_X-8.0f+2>=planebulletsx[i])
			{
				healthlimit--;
				planebulletsx[i]=-20;
			}

			Matrices.model = glm::mat4(1.0f);
			glm::mat4 planebull = glm::translate (glm::vec3(planebulletsx[i],planebulletsy[i],0.0f)); 
			Matrices.model*= planebull;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			draw3DObject(planebullets[i]);
			planebulletacc[i]+=0.008;
			planebulletsy[i]-=planebulletacc[i];
		}
	}

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
	redbar[0]=255;
	redbar[1]=238;
	redbar[2]=205;
	redbar[3]=205;greenbar[3]=55;
	redbar[4]=238;greenbar[4]=64;
	redbar[5]=238;greenbar[5]=118;
	redbar[6]=255;greenbar[6]=165;
	redbar[7]=255;greenbar[7]=193;bluebar[7]=37;
	redbar[8]=255;greenbar[8]=215;	
	redbar[9]=238;greenbar[9]=238;
	redbar[10]=255;greenbar[10]=255;
	redbar[11]=192;greenbar[11]=255;bluebar[11]=62;
	redbar[12]=173;greenbar[12]=255;bluebar[12]=47;
	redbar[13]=127;greenbar[13]=255;
	greenbar[14]=205;
	greenbar[15]=238;
	greenbar[16]=255;







	planebulletlimit=rand()%5;
	for(int i=0;i<=planebulletlimit;i++)
	{
		planebulletsx[i]=(rand()%20)-10.0f;
		planebulletsy[i]=PLANE_POS_Y-0.5;
		planebulletacc[i]=0;
	}













	GROUND = createRectangle(.545f,.27f,.07f, .134f,.545f,.134f, -10,-9,  10,-9,  -10,-8,  10,-8); 
	SKY = createRectangle(.596f,.96f,1,.117f,.564f,1, -10,-9.0f,  10,-9.0f,  -10,10,  10,10);                  //colours bottom first.

	GLfloat posx=-8.5f;
	for(int i=0;i<5;i++)
	{
		WHEELS[i]=createCircle(posx,-7.8f,0.0f,.2,108);
		posx+=0.45;
	}
	TANK_SPROCKET_LEFT = createCircle(-8.75f,-7.6f,0.0f,.12f,108);
	TANK_SPROCKET_RIGHT = createCircle(-6.4f,-7.6f,0.0f,.12f,108);

	TANK_BODY_UPPER= createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, -9+.2f,-7.5 , -6.6+0.2f,-7.5, -8.7f+0.2f,-7.3f, -6.9f+0.2f,-7.3);
	TANK_BODY_LOWER = createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, -9+.2f,-7.5 , -6.6+0.2f,-7.5, -8.7f+0.2f,-7.8f, -6.9f+0.2f,-7.8f);
	TANK_TRACK = createRectangle(0.666f,0.666f,0.666f,0.666f,0.666f,0.666f, -9.0f,-7.5f, -6.2f,-7.5f, -8.7f,-7.96f, -6.5f,-7.96f);
	TANK_TURRET_BASE = createRectangle(0.156,0.156,0.156,0.156,0.156,0.156, -8.5+.8f,-7.3 , -7.6+0.8f,-7.3, -8.4f+0.8f,-7.1f, -7.7f+0.8f,-7.1f);
	TANK_TURRET = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,  0.0f,-0.2f, 1.0f,-0.1f, 0.0f,0.2f, 1.0f,0.1f);
	TANK_TURRET_END =createRectangle(0.15f,0.15f,0.15f,0.15f,0.15f,0.15f, 0.8f,-0.2f, 1.0f,-0.2f, 0.8f,0.2f, 1.0f,0.2f);

	TANK_GUN_BASE = createRectangle(0.15f,0.15f,0.15f,0.15f,0.15f,0.15f, 0.0f,0.0f, 0.4f,0.0f, 0.1f,0.6f, 0.3f,0.6f);


	TANK_GUN_HANDLE = createRectangle(0.2f,0.2f,0.2f,0.2f,0.2f,0.2f, -0.25f,-0.15f, 0.25f,-0.15f, -0.25f,0.15f, 0.25f,0.15f);
	TANK_GUN = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0.0f,-0.1f, 1.3f,-0.05f, 0.0f,0.1f, 1.3f,0.05f); 
	TANK_GUN_BARREL = createRectangle(0.3f,0.3f,0.3f,0.3f,0.3f,0.3f, 1.2f,-0.1f, 1.4f,-0.1f, 1.2f,0.1f, 1.4f,0.1f); 
	TANK_GUN_TRIGGER = createRectangle(0.1f,0.1f,0.1f,0.1f,0.1f,0.1f, -0.25f,-0.3f, -0.2f,-0.3f, -0.25f,0.3f, -0.2f,0.3f); 

	BALL = createCircle(1.5f,0.0f,0.0f,.2f,108);
	ENEMY_WALL_1 = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,  -0.1f,0.0f, 0.1f,0.0f, -0.1f,2.0f, 0.1f,2.0f);

	float l_add = 0.0f;
	for(int i=0;i<13;i++)
	{
		CHAINS[i]=createRectangleOutline(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, -0.1f+l_add,0.0f, 0.0f+l_add,-0.1f, 0.0f+l_add,.1f, 0.1f+l_add,-0.0f);
		l_add+=0.2f;
	}


	ENEMY_TURRET_BASE = createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, -0.8f,0.0f,  0.8f,0.0f,  -0.6f,0.3f,  0.6f,0.3f);
	ENEMY_TURRET_RING = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, -0.3f,0.3f, 0.3f,0.3f, -0.1f,0.6f, 0.1f,0.6f);
	ENEMY_TURRET_BODY = createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, -0.4f,0.6f, 0.4f,0.6f, -0.2f,1.0, 0.35f,1.0f);
	ENEMY_TURRET_GUN = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, -0.9f,0.75f, -0.2f,0.7f, -0.9f,.85f, -0.2f,.9f);

	ENEMY_PLATFORM = createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, -4.0f,0.0f,  0.0f,0.0f,  -4.4f,0.3f,  0.0f,0.3f);

	ENEMY_PLATFORM2 = createRectangle(0.317f,0.317f,0.317f,0.317f,0.317f,0.317f, 1.5f,0.0f,  3.5f,0.0f,  1.5f,0.3f,  3.5f,0.3f);
	ENEMY_PLATFORM3 = createRectangle(0.517f,0.517f,0.517f,0.517f,0.517f,0.517f, 0.0f,0.0f,  1.5f,0.0f,  0.0f,0.3f,  1.5f,0.3f);
	ENEMY_PLATFORM_WALL = createRectangle(0.2f,0.2f,0.2f,0.2f,0.2f,0.2f, -0.5f,-0.5f,  0.5f,-0.5f,  -0.5f,0.5f,  0.5f,0.5f);

	ENEMY_PLATFORM_WALL2 = createRectangle(0.4f,0.4f,0.4f,0.4f,0.4f,0.4f, -0.5f,-0.5f,  0.5f,-0.5f,  -0.5f,0.5f,  0.5f,0.5f);

	ENEMY_PLATFORM_WALL3 = createRectangle(0.6f,0.6f,0.6f,0.6f,0.6f,0.6f, -0.5f,-0.5f,  0.5f,-0.5f,  -0.5f,0.5f,  0.5f,0.5f);
	DUMMYBOX = createRectangle(1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, -.1f,-0.1f,  0.1f,-0.1f,  -.1f,0.1f,  .1f,0.1f);
	PLANE_NOSE = createTriangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, -.5f,0.0f , 0.0f,0.15f, 0.0f,-0.15);

	PLANE_BODY_1 = createRectangle(0.4f,0.4f,0.4f,0.4f,0.4f,0.4f,  0.0f,-0.15f, 0.4f,-0.2f, 0.0f,0.15f, 0.4f,0.2f);
	PLANE_BODY_2 = createRectangle(0.5f,0.5f,0.5f,0.5f,0.5f,0.5f, 0.4f,-0.18f, 1.0f,-0.18f, 0.4f,0.18f, 1.0f,0.18f);
	PLANE_BODY_3 = createRectangle(0.2f,0.2f,0.2f,0.2f,0.2f,0.2f,  1.0f,-0.2f, 1.4f,-0.15f, 1.0f,.2f, 1.4f,0.1f);
	PLANE_COCKPIT = createCircle(0.2f,0.06f ,0.0f, 0.25f,108);

	PLANE_TAIL  = createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 1.0f,0.2f, 1.4f,0.1f , 1.3f,0.6f, 1.5f,0.6f);

	PLANE_WING = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,  0.7f,-0.3f, 1.0f,-0.3f, 0.5f,0.0f, .8f,0.0f);

	for(int i=0;i<1000;i++)
		TANK_BULLETS[i] = createCircle(1.5f,0.0f,0.0f,0.1,108);

	SMOKE_UPPER = createTriangle(1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, 1.5f,0.0f , 1.5f,0.2f, 1.7f,0.1);
	SMOKE_LOWER = createTriangle(1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, 1.5f,0.0f , 1.5f,-0.2f, 1.7f,-0.1);


	for(int i=0;i<10000;i++)
	{
		enemybullets[i] = createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.0f, 0.8f,0.0f , 0.0f,0.2f, 0.8f,0.2f);
		//planebullets[i] = createCircle(0.0f,0.0f,0.0f,1.0f,108); 
	}

	tankshield = createCircle(0.0f,0.0f,0.0f,1.8,108);
	ENEMY_STAND_WALL =createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.0f, 5.0f,0.0f , 0.0f,0.4f, 5.0f,0.4f);


	enemywallhinge1=createRectangle(0.1,0.1,0.1,0.1,0.1,0.1, -0.2f,-0.1f, 0.2f,-0.1f , -0.3f,0.3f, 0.3f,0.3f);
	enemywallhinge2=createCircle(0.0,0.0,0.0,0.08f,108);


	energybaroutline=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.0f, 1.0f,0.0f , 0.0f,8.0f, 1.0f,8.0f);
	energybar=createRectangle(135.0/255.0,206/255.0,250/255.0,0,178.0/255.0,238.0/255.0, 0.1f,0.2f, 0.9f,0.2f , 0.1f,7.8f, 0.9f,7.8f);

	healthbaroutline=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.0f, 6.5f,0.0f , 0.0f,1.2f, 6.5f,1.2f);
	healthbarback=createRectangle(1,1,1,1,1,1, 0.2f,0.1f, 6.3f,0.1f , 0.2f,1.1f, 6.3f,1.1f);
	//healthbarback=createRectangle(0,178.0/255.0,238.0/255.0, 0,178.0/255.0,238.0/255.0, 0.2f,0.1f, 6.3f,0.1f , 0.2f,1.1f, 6.3f,1.1f);
	float lenbar=0;
	for(int i=0;i<21;i++)
	{
		healthbars[i]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.4f+lenbar,0.2f, 0.65f+lenbar,0.2f , 0.4f+lenbar,1.0f, 0.65f+lenbar,1.0f);
		lenbar+=0.35;
	}

	for(int i=0;i<10;i++)
		planebullets[i]=createCircle(0.0,0.0,0.0,0.2f,108);








	sevenseg[0]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.0f, 0.8f,0.0f , 0.1f,0.1f, 0.7f,0.1f);
	sevenseg[1]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,0.1f, 0.1f,0.2f , 0.0f,.9f, 0.1f,0.8f);
	sevenseg[2]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.0f,1.0f, 0.1f,1.1f , 0.0f,1.8f, 0.1f,1.7f);
	sevenseg[3]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.1f,1.8f, 0.7f,1.8f , 0.0f,1.9f, 0.8f,1.9f);
	sevenseg[4]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.7f,1.1f, 0.8f,1.0f , 0.7f,1.7f, 0.8f,1.8f);
	sevenseg[5]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.7f,0.2f, 0.8f,0.1f , 0.7f,0.8f, 0.8f,0.9f);
	sevenseg[6]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.1f,0.95f, 0.7f,0.95f , 0.2f,1.0f, 0.6f,1.0f);
	sevenseg[7]=createRectangle(0.0,0.0,0.0,0.0,0.0,0.0, 0.2f,0.9f, 0.6f,0.9f , 0.1f,0.95f, 0.7f,0.95f);


	for(int i=0;i<6;i++)
		sevensegdecoder[0].push_back(i);
	sevensegdecoder[1].push_back(4);
	sevensegdecoder[1].push_back(5);
	
	sevensegdecoder[2].push_back(3);
	sevensegdecoder[2].push_back(4);
	sevensegdecoder[2].push_back(6);
	sevensegdecoder[2].push_back(7);
	sevensegdecoder[2].push_back(1);
	sevensegdecoder[2].push_back(0);

	sevensegdecoder[3].push_back(3);
	sevensegdecoder[3].push_back(4);
	sevensegdecoder[3].push_back(6);
	sevensegdecoder[3].push_back(7);
	sevensegdecoder[3].push_back(5);
	sevensegdecoder[3].push_back(0);

	sevensegdecoder[4].push_back(2);
	sevensegdecoder[4].push_back(6);
	sevensegdecoder[4].push_back(7);
	sevensegdecoder[4].push_back(4);
	sevensegdecoder[4].push_back(5);


	sevensegdecoder[5].push_back(3);
	sevensegdecoder[5].push_back(2);
	sevensegdecoder[5].push_back(6);
	sevensegdecoder[5].push_back(7);
	sevensegdecoder[5].push_back(5);
	sevensegdecoder[5].push_back(0);
	
	
	sevensegdecoder[6].push_back(3);
	sevensegdecoder[6].push_back(2);
	sevensegdecoder[6].push_back(6);
	sevensegdecoder[6].push_back(7);
	sevensegdecoder[6].push_back(5);
	sevensegdecoder[6].push_back(1);
	sevensegdecoder[6].push_back(0);
	
	
	sevensegdecoder[7].push_back(3);
	sevensegdecoder[7].push_back(4);
	sevensegdecoder[7].push_back(5);

	for(int i=0;i<8;i++)
		sevensegdecoder[8].push_back(i);
	
	sevensegdecoder[9].push_back(2);
	sevensegdecoder[9].push_back(3);
	sevensegdecoder[9].push_back(4);
	sevensegdecoder[9].push_back(6);
	sevensegdecoder[9].push_back(7);
	sevensegdecoder[9].push_back(5);
	
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	//reshapeWindow (window, width, height);
	glClearColor (1.0f, 1.0f, 1.0f, 1.0f); // R, G, B, A
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
	int width = 800;
	int height = 800;
	GLFWwindow* window = initGLFW(width, height);
	initGL (window, width, height);
	double last_update_time = glfwGetTime(), current_time;
	while (!glfwWindowShouldClose(window)) 
	{
		if(quitgame)
			quit(window);
		glfwGetCursorPos(window,&CURSOR_X,&CURSOR_Y);
		CURSOR_X=CURSOR_X/40.0;
		if(camera_up)
		{
			orthodowny+=0.05f;
			orthoupy+=0.05f;
		}
		if(camera_down)
		{
			orthodowny-=0.05f;
			orthoupy-=0.05f;
		}

		if(camera_left)	
		{
			ortholeftx-=0.05;
			orthorightx-=0.05;
		}

		if(camera_right)
		{
			ortholeftx+=0.05f;
			orthorightx+=0.05f;
		}

		if(camera_zoom)
		{
			orthoupy-=0.05f;
			orthodowny+=0.05f;
			ortholeftx+=0.05f;
			orthorightx-=0.05f;
		}

		if(camera_zoomout)
		{
			orthoupy+=0.05f;
			orthodowny-=0.05f;
			ortholeftx-=0.05f;
			orthorightx+=0.05f;
		}
		reshapeWindow (window, width, height);
		CURSOR_Y=CURSOR_Y/40.0;
		CURSOR_X-=10.0f;
		CURSOR_Y-=10.0f;
		CURSOR_Y*=-1.0f;
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 0.5) 
		{ 	// atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
