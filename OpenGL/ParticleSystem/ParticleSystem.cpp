/*
   Particle system and reflection 
*/

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "math_3d.h"
#include <vector>

using namespace std;

GLuint VBO;
GLuint VertexShaderId, FragmentShaderId, ProgramId;

unsigned int tex, snow;

GLfloat fogDensity = 1.0f;
GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

//----------------------PARTICLE SYSTEM--------------------
const int particleCount = 5000;
GLfloat texture[10];
typedef struct 
{
	float life, decay, r,g,b, x, y, z, xs, ys, zs;
}part;

part part1[particleCount];
	
void startPart(int i)		//initialize the particle system
{
	part1[i].life = (float)(rand()%500000)/50000.0;;
	part1[i].decay = 0.001;
	part1[i].r=0.7;part1[i].g=0.7; part1[i].b=1.0;
	part1[i].x=0.0;part1[i].y=0.0; part1[i].z=4.0;
	part1[i].xs =0.005-(float)(rand()%100)/10000.0;part1[i].ys = 0.005-(float)(rand()%100)/10000.0; part1[i].zs = -(float)(rand()%200)/100000.0;
}

void carryPart()			//update the value of particles at a particular time
{
	for(int i=0; i<particleCount; i++)
	{
		part1[i].life -= part1[i].decay;
		part1[i].x    += part1[i].xs;
		part1[i].y    += part1[i].ys;
		part1[i].z    += part1[i].zs;
		part1[i].zs -= 0.00006;
	}
}

void drawpart()				//drawing the particles on OpenGL window - called from display()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, snow);
	for(int i=0; i<particleCount; i++)
	{
		if(part1[i].z < -1.0) part1[i].life = 0.0;
		if(part1[i].life>0.0)
		{
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0.0,1.0); glVertex3f(part1[i].x+0.01, part1[i].y+0.01, part1[i].z+0.000);
				glTexCoord2f(0.0,0.0); glVertex3f(part1[i].x-0.01, part1[i].y+0.01, part1[i].z+0.000);
				glTexCoord2f(1.0,1.0); glVertex3f(part1[i].x+0.01, part1[i].y-0.01, part1[i].z+0.000);
				glTexCoord2f(1.0,0.0); glVertex3f(part1[i].x-0.01, part1[i].y-0.01, part1[i].z+0.000);
			glEnd();
		}
		else startPart(i);
	}
	carryPart();
	glDisable(GL_TEXTURE_2D);
}


//----------------------PARTICLE SYSTEM END--------------------


unsigned int loadTexture(const char* filename)
{
	SDL_Surface* img = SDL_LoadBMP(filename);
	unsigned int id;

	glGenTextures(1, &id);  //Generate Texture
	glBindTexture(GL_TEXTURE_2D, id);

	//use Texture - No Mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img->w, img->h, 0, GL_BGR, GL_UNSIGNED_BYTE, img->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//use Texture - With Mipmaps
	/*gluBuild2DMipmaps( GL_TEXTURE_2D, 3, img->w, img->h, GL_BGR, GL_UNSIGNED_BYTE, img->pixels );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/

	SDL_FreeSurface(img);

	return id;
}

/*
Renders a pyramid (tetrahedron) and its reflection (another tetrahedron
which is inverted and clipped using stencils). drawpart() called from this
function to draw the particles falling from above.
*/
static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	
	glRotatef(1.0, 0.0, 0.0, 1.0);

	drawpart();
	
	glUseProgram(ProgramId);
	GLint posAttrib = glGetAttribLocation(ProgramId, "in_Position");
	
    glEnableVertexAttribArray(posAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(posAttrib, 3 , GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

	GLint texAttrib = glGetAttribLocation(ProgramId, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glBindTexture(GL_TEXTURE_2D, tex);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                       5*sizeof(float), (void*)(3*sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, 18);

	glDisableVertexAttribArray(posAttrib);
	glDisableVertexAttribArray(texAttrib);
	glUseProgram(0);
	

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_STENCIL_TEST);

    // Draw floor
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF); // Write to stencil buffer
    glDepthMask(GL_FALSE); // Don't write to depth buffer
    glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

	glBegin(GL_QUADS);

		glVertex3f(-2.0, -2.0f, -1.0f);
		glVertex3f(-2.0, 2.0f, -1.0f);
		glVertex3f(2.0, 2.0f, -1.0f);
		glVertex3f(2.0, -2.0f, -1.0f);
		
	glEnd();
	
	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
    glStencilMask(0x00); // Don't write anything to stencil buffer
    glDepthMask(GL_TRUE); 
	

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor3f(0.4f, 0.4f, 0.4f);
	glBegin(GL_TRIANGLES);
		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
    
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f( 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, 0.0f, -3.0f);

		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f( 0.5f, 1.0f);
		glVertex3f(0.0f, 0.0f, -3.0f);
		glTexCoord2f( 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
    
		glTexCoord2f( 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, 0.0f, -3.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, 0.0f, -3.0f);
	
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_STENCIL_TEST);

	

/*	glBegin(GL_QUADS);
		glVertex3f(-2.0, -2.0f, -1.0f);
		glVertex3f(-2.0, 2.0f, -1.0f);
		glVertex3f(2.0, 2.0f, -1.0f);
		glVertex3f(2.0, -2.0f, -1.0f);
	glEnd();
	
	*/


	
    glutSwapBuffers();
	glutPostRedisplay();
}

//Shader Creation
static void CreateShaders(void)
{
    GLenum ErrorCheckValue = glGetError();
     
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	ifstream ifs("VertexShader.vert");
	std::string str( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
	GLchar *cstr = new GLchar[str.size() + 1];
	strcpy(cstr, str.c_str());
	const GLchar *vertexShader = cstr;
	
	//cout<<"Vertex Shader : "<<endl;
	//printf("%s\n",cstr); 
	glShaderSource(VertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(VertexShaderId);
    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    ifstream ifsf("FragmentShader.frag");
	std::string strf( (std::istreambuf_iterator<char>(ifsf) ),
                       (std::istreambuf_iterator<char>()    ) );
	GLchar *cstrf = new GLchar[strf.size() + 1];
	strcpy(cstrf, strf.c_str());
	const GLchar *fragmentShader = cstrf;

	//cout<<"Fragment Shader : "<<endl;
	//printf("%s\n",cstrf);
	glShaderSource(FragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(FragmentShaderId);
    ProgramId = glCreateProgram();
        glAttachShader(ProgramId, VertexShaderId);
        glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
   // glUseProgram(ProgramId);
	cout<<"PROGRAM ID:"<<ProgramId<<endl;
	GLint status;
	    glGetProgramiv (ProgramId, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(ProgramId, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

	cout<<"error not reached"<<endl;
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not create the shaders: %s \n",
            gluErrorString(ErrorCheckValue)
        );
 
        exit(-1);
    }

	cout<<"ShaderCreated"<<endl;
}

//vertices Information for pyramid
static void CreateVertexBuffer()
{
    GLfloat Vertices[] =
	{
	
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    
	1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.5f, 1.0f,

	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.5f, 1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    
	1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.5f, 1.0f,
	1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

	1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.5f, 1.0f

	};

 	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_STENCIL);
    glutInitWindowSize(768, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 02");

	printf("%s \n", glGetString(GL_VERSION));
	
	  glEnable (GL_DEPTH_TEST);
	

	//----------Fog Enable-------------
	/*glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, fogDensity);
	glHint(GL_FOG_HINT, GL_NICEST);*/
	
	//----------Light Enable-----------
    //glEnable (GL_LIGHTING);
    //glEnable (GL_LIGHT0);
	

	glEnable(GL_POINT_SPRITE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 3.0, -3.0, 3.0, -3.0, 3.0, -3.0);
	gluLookAt(0.0, 0.0, 0.0,
			  5.0, 5.0 , 3.0,
			  0.0, 0.0, -1.0);


	for(int i=0; i<particleCount; i++) startPart(i);
	//initParticles();
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1; 
    }
	CreateVertexBuffer();
	  glutDisplayFunc(RenderSceneCB);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	tex = loadTexture("pyramid.bmp");
	snow = loadTexture("snowflake.bmp");
	std::cout<<"tex : "<<tex<<std::endl;
	  CreateShaders();
    glutMainLoop();

    return 0;
}

