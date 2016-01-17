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

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;

int N = 20;

float TRANSLATE_TANK_BY = 0.0f;
float TANK_POS_X=0.0f;
float TURRET_ANGLE = 40;
float ROTATE_TURRET_BY = 0;

float VELOCITY = .5f;
float BALL_POS_X = TANK_POS_X;
float BALL_POS_Y = 0.0f;
float DECELERATION = 0.1f;
float BALL_VEL_X = 0.0f;
float BALL_VEL_Y = 0.0f;


int SHOW_BALL = 1;
int SHOW_CHAINS = 1;


VAO * objects[100],*GROUND,*SKY,*WHEELS[20],*TANK_BODY_UPPER,*TANK_BODY_LOWER,*TANK_TURRET_BASE,*TANK_SPROCKET_LEFT,*TANK_SPROCKET_RIGHT,*TANK_TRACK,*TANK_TURRET,*TANK_TURRET_END,*BALL,*ENEMY_WALL_1,*CHAINS[20];

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
		    case GLFW_KEY_UP:
			    ROTATE_TURRET_BY=0;
			    break;
		    case GLFW_KEY_DOWN:
			    ROTATE_TURRET_BY=0;
			    break;
		    case GLFW_KEY_LEFT:
			    TRANSLATE_TANK_BY = 0.0f;
			    break;
		    case GLFW_KEY_RIGHT:
			   TRANSLATE_TANK_BY = 0.0f; 
			    break;
		    default:
			    break;
        }
    }
    else if (action == GLFW_PRESS) {
	    switch (key) 
	    {
		    case GLFW_KEY_UP:
			   ROTATE_TURRET_BY += 1.0f; 
			    break;
		    case GLFW_KEY_DOWN:
			    ROTATE_TURRET_BY += -1;
			    break;
		    case GLFW_KEY_LEFT:
			    TRANSLATE_TANK_BY = -0.1f;
			    break;
		    case GLFW_KEY_RIGHT:
			    TRANSLATE_TANK_BY = 0.1f;
			    break;
		    case GLFW_KEY_SPACE:
			    BALL_POS_Y = 0.0f;
			    BALL_POS_X = TANK_POS_X;
			    BALL_VEL_X = VELOCITY * cos( TURRET_ANGLE * M_PI/180.0);
			    BALL_VEL_Y = VELOCITY * sin( TURRET_ANGLE * M_PI/180.0);
		            DECELERATION = BALL_VEL_Y/15.0;
			    SHOW_BALL = 1;
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
                triangle_rot_dir *= -1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
                rectangle_rot_dir *= -1;
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
    Matrices.projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 500.0f);
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
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram (programID);
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  glm::vec3 target (0, 0, 0);
  glm::vec3 up (0, 1, 0);
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 VP = Matrices.projection * Matrices.view;
  glm::mat4 MVP;	

  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translateRectangle = glm::translate (glm::vec3(0.0f, -1.0f, 0.0f)); 
  Matrices.model*=translateRectangle;
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(GROUND);
	  
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translatesky = glm::translate (glm::vec3(0.0f, 0.0f, 0.0f)); 
  Matrices.model*=translatesky;
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(SKY);

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
  

  TANK_POS_X += TRANSLATE_TANK_BY;

  if(SHOW_BALL)
  {
	  Matrices.model = glm::mat4(1.0f);
	  glm::mat4 translateball = glm::translate (glm::vec3(BALL_POS_X-6.9f, BALL_POS_Y-6.9f-1.0f, 0.0f)); 
	  Matrices.model*=translateball*rotatetankturret;
	  MVP = VP * Matrices.model;
	  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	  draw3DObject(BALL);
  }

  BALL_POS_X += BALL_VEL_X;
  BALL_POS_Y += BALL_VEL_Y;
  BALL_VEL_Y-=DECELERATION;
  if(BALL_POS_Y<=-2.6)
	   SHOW_BALL = 0;

  if(BALL_POS_X-6.9f >=7.5f && BALL_POS_X <= 10.0f && BALL_POS_Y <= -6.7)
  {
	  SHOW_CHAINS = 0;
  }

  Matrices.model = glm::mat4(1.0f);
  glm::mat4 rotateenemywall1 = glm::rotate((float)(10.0*M_PI/180.0f), glm::vec3(0,0,1));
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
	TANK_TURRET_BASE = createRectangle(0.156,0.156,0.156,0.156,0.156,0.156, -8.7+.8f,-7.3 , -7.6+0.8f,-7.3, -8.6f+0.8f,-7.1f, -7.7f+0.8f,-7.1f);
	TANK_TURRET = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,  0.0f,-0.2f, 1.0f,-0.1f, 0.0f,0.2f, 1.0f,0.1f);
	TANK_TURRET_END =createRectangle(0.15f,0.15f,0.15f,0.15f,0.15f,0.15f, 0.8f,-0.2f, 1.0f,-0.2f, 0.8f,0.2f, 1.0f,0.2f);

	BALL = createCircle(1.5f,0.0f,0.0f,.3f,108);
	ENEMY_WALL_1 = createRectangle(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,  -0.2f,0.0f, 0.2f,0.0f, -0.2f,2.0f, 0.2f,2.0f);
	
	float l_add = 0.0f;
	for(int i=0;i<13;i++)
	{
		CHAINS[i]=createRectangleOutline(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, -0.1f+l_add,0.0f, 0.0f+l_add,-0.1f, 0.0f+l_add,.1f, 0.1f+l_add,-0.0f);
		l_add+=0.2f;
	}

	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	reshapeWindow (window, width, height);
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
