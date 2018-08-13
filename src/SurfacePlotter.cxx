/*!
   \file    SurfacePlotter.cxx
   \author  msoderen
   \date    Aug 10, 2018
   \version 0.0.1

   \brief Header-file for the Template class.

   \section DESCRIPTION

   This file contains the declaration of the Template class.

 */
#include "SurfacePlotter.h"

namespace SurfacePlotting {

void DrawCallback(){
	plotter->Draw();
}
void TimerCallback(int i){
	plotter->Timer(i);

}
void KeyboardCallback(unsigned char key, int x_disp, int y_disp){
	plotter->Keyboard(key,x_disp,y_disp);
}

void MouseMovementCallback(int x,int y){
	plotter->MouseMovement(x,y);
}

void MousePressCallback(int button, int state, int x, int y) {

	plotter->MousePress(button,state,x,y);

}

SurfacePlotter::SurfacePlotter(){
	plotter=this;
}
/*!
 * \fn ~3dSurfacePlotter()
 * \brief destructor for 3dSurfacePlotter class.
 */
SurfacePlotter::~SurfacePlotter(){}

void SurfacePlotter::show(const std::size_t& width,const std::size_t& height){
	m_width=width;
	m_height=height;
	//get the max size of a texture in the graphics card
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&m_maxTextureSize);
	int argc = 1;
	char *argv[1] = {(char*)"Something"};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(m_width, m_height);
	glutCreateWindow("3D surface plotter by Martin Soderen");
	glutDisplayFunc(DrawCallback);
	glutPassiveMotionFunc(MouseMovementCallback);
	glutMouseFunc(MousePressCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutTimerFunc(20, TimerCallback, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for(auto element : m_surfaces){
		element->init();
	}
	glutMainLoop();

}
void SurfacePlotter::initOpenGl(){

}

void SurfacePlotter::Draw(){

	//just for fps
	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	std::size_t timeSinceLastFrame=nanoseconds.count()-timeLastFrame;
	timeLastFrame=nanoseconds.count();
	double fps=1.0/(static_cast<float>(timeSinceLastFrame)/1000000000.0);
	//std::cout<<"fps: "<<fps<<std::endl;
	bool updateRequired=false;
	for(auto element:m_surfaces){
		if(element->updateRequired()){
			updateRequired=true;
		}
	}
	if(!updateRequired){
		glutPostRedisplay();
		return;
	}

	// clear first
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

for(auto element:m_surfaces){
	// use the shader program
	glUseProgram(element->m_shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(element->m_shaderProgram, "projMatrix"), 1,
	GL_TRUE, element->m_frustum_matrix.m);
	glUniformMatrix4fv(glGetUniformLocation(element->m_shaderProgram, "cameraMatrix"), 1,
	GL_TRUE, lookAtv(element->m_camera, element->m_looking, element->m_up).m);

	printError("While setting up uniform variables");

	glActiveTexture(GL_TEXTURE0);
	printError("draw1");
	glUniform1i(glGetUniformLocation(element->m_shaderProgram, "tex"),
	GL_TEXTURE0);
	printError("draw1.1");
	if(element->update()){
	}
	glUniform1f(glGetUniformLocation(element->m_shaderProgram, "offset"),  element->m_offset);
	printError("draw2");
	glBindTexture(GL_TEXTURE_2D, element->m_tex);
	glBindVertexArray(element->m_vao);

	// draw
	glDrawElements(GL_TRIANGLES, 6*(element->m_rows)*(element->m_cols), GL_UNSIGNED_INT, 0);

}
	printError("draw3");
	// bind the vao

	glutSwapBuffers();
	// check for errors
	printError("draw");
	glutPostRedisplay();

}
void SurfacePlotter::Timer(int i){
	//glutTimerFunc(1, TimerCallback, i);
	//glutPostRedisplay();
}

void SurfacePlotter::Keyboard(unsigned char key, int x_disp, int y_disp){

}

void SurfacePlotter::MouseMovement(int x,int y){

}
void SurfacePlotter::MousePress(int button, int state, int x,int y){

}

}


