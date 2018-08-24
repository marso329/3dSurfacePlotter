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

void DrawCallback() {
	plotter->Draw();
}
void TimerCallback(int i) {
	plotter->Timer(i);

}
void KeyboardCallback(unsigned char key, int x_disp, int y_disp) {
	plotter->Keyboard(key, x_disp, y_disp);
}

void KeyboardSpecialCallback(int key, int x_disp, int y_disp) {
	plotter->KeyboardSpecial(key, x_disp, y_disp);
}

void MouseMovementCallback(int x, int y) {
	plotter->MouseMovement(x, y);
}

void MousePressCallback(int button, int state, int x, int y) {

	plotter->MousePress(button, state, x, y);

}

SurfacePlotter::SurfacePlotter() {
	plotter = this;
}
/*!
 * \fn ~3dSurfacePlotter()
 * \brief destructor for 3dSurfacePlotter class.
 */
SurfacePlotter::~SurfacePlotter() {
}

void SurfacePlotter::show(const std::size_t& width, const std::size_t& height) {
	m_width = width;
	m_height = height;
	//get the max size of a texture in the graphics card
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
	int argc = 1;
	char *argv[1] = { (char*) "Something" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(m_width, m_height);
	glutCreateWindow("3D surface plotter by Martin Soderen");
	glutDisplayFunc(DrawCallback);
	glutMotionFunc(MouseMovementCallback);
	glutMouseFunc(MousePressCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(KeyboardSpecialCallback);
	glutTimerFunc(20, TimerCallback, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (auto element : m_surfaces) {
		element->init();
	}
	glutMainLoop();

}
void SurfacePlotter::initOpenGl() {

}

void SurfacePlotter::Draw() {

	//just for fps
	std::chrono::time_point<std::chrono::high_resolution_clock> now =
			std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
			duration);
	std::size_t timeSinceLastFrame = nanoseconds.count() - timeLastFrame;
	timeLastFrame = nanoseconds.count();
	double fps = 1.0 / (static_cast<float>(timeSinceLastFrame) / 1000000000.0);
	//std::cout<<"fps: "<<fps<<std::endl;
	bool updateRequired = false;
	for (auto element : m_surfaces) {
		if (element->updateRequired()) {
			updateRequired = true;
		}
	}
	if (!updateRequired) {
		glutPostRedisplay();
		return;
	}

	// clear first
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto element : m_surfaces) {
		// use the shader program
		glUseProgram(element->m_shaderProgram);

		glUniformMatrix4fv(
				glGetUniformLocation(element->m_shaderProgram, "projMatrix"), 1,
				GL_TRUE, element->m_frustum_matrix.m);
		glUniformMatrix4fv(
				glGetUniformLocation(element->m_shaderProgram, "cameraMatrix"),
				1,
				GL_TRUE,
				lookAtv(element->m_camera, element->m_looking, element->m_up).m);

		printError("While setting up uniform variables");

		glActiveTexture(GL_TEXTURE0);
		printError("draw1");
		glUniform1i(glGetUniformLocation(element->m_shaderProgram, "tex"),
		GL_TEXTURE0);
		printError("draw1.1");
		if (element->update()) {
		}
		glUniform1f(glGetUniformLocation(element->m_shaderProgram, "offset"),
				element->m_offset);
		printError("draw2");
		glBindTexture(GL_TEXTURE_2D, element->m_tex);
		glBindVertexArray(element->m_vao);

		// draw
		glDrawElements(GL_TRIANGLES, 6 * (element->m_rows) * (element->m_cols),
		GL_UNSIGNED_INT, 0);

	}
	printError("draw3");
	// bind the vao

	glutSwapBuffers();
	// check for errors
	printError("draw");
	glutPostRedisplay();

}
void SurfacePlotter::Timer(int i) {
	//glutTimerFunc(1, TimerCallback, i);
	//glutPostRedisplay();
}

void SurfacePlotter::KeyboardSpecial(int key, int x_disp, int y_disp){
	for (auto element : m_surfaces) {
		vec3 lookingdir = Normalize(VectorSub(element->m_looking, element->m_camera));
		vec3 strafe = Normalize(CrossProduct(lookingdir, element->m_up));
		double distance = MATH::distance(element->m_camera, element->m_looking);
		double degree = MATH::degree(lookingdir.x, lookingdir.z);
		double x = MATH::cos(degree);
		double y = MATH::sin(degree);
		//forward
	if (key == GLUT_KEY_UP) {
		element->m_camera.x += lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_looking.x += lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_camera.z += lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_looking.z += lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
	//backwards
	} else if (key == GLUT_KEY_DOWN) {
		element->m_camera.x -= lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_looking.x -= lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_camera.z -= lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
		element->m_looking.z -= lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
	} else if (key == GLUT_KEY_LEFT) {
		degree -= 10;
		x = MATH::cos(degree);
		y = MATH::sin(degree);
		element->m_looking.x = element->m_camera.x + x * distance;
		element->m_looking.z = element->m_camera.z + y * distance;
	} else if (key == GLUT_KEY_RIGHT) {
		degree += 10;
		x = MATH::cos(degree);
		y = MATH::sin(degree);
		element->m_looking.x = element->m_camera.x + x * distance;
		element->m_looking.z = element->m_camera.z + y * distance;
	//strafe left
	}
	}
}
void SurfacePlotter::Keyboard(unsigned char key, int x_disp, int y_disp) {
	// 111=forward, 113=left, 114=right,116=back,25=w,38=a,40=d,39=s
	for (auto element : m_surfaces) {
		vec3 lookingdir = Normalize(VectorSub(element->m_looking, element->m_camera));
		vec3 strafe = Normalize(CrossProduct(lookingdir, element->m_up));
		double distance = MATH::distance(element->m_camera, element->m_looking);
		double degree = MATH::degree(lookingdir.x, lookingdir.z);
		double x = MATH::cos(degree);
		double y = MATH::sin(degree);
		//forward
		if (key == 'w') {
			element->m_camera.x += lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.x += lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_camera.z += lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.z += lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
		//backwards
		} else if (key == 's') {
			element->m_camera.x -= lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.x -= lookingdir.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_camera.z -= lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.z -= lookingdir.z * DISTANCE_TO_MOVE_KEYBOARD;
		} else if (key == 113) {
			degree -= 10;
			x = MATH::cos(degree);
			y = MATH::sin(degree);
			element->m_looking.x = element->m_camera.x + x * distance;
			element->m_looking.z = element->m_camera.z + y * distance;
		} else if (key == 114) {
			degree += 10;
			x = MATH::cos(degree);
			y = MATH::sin(degree);
			element->m_looking.x = element->m_camera.x + x * distance;
			element->m_looking.z = element->m_camera.z + y * distance;
		//strafe left
		} else if (key == 'a') {
			element->m_camera.x -= strafe.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.x -= strafe.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_camera.z -= strafe.z * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.z -= strafe.z * DISTANCE_TO_MOVE_KEYBOARD;
		//strafe right
		} else if (key == 'd') {
			element->m_camera.x += strafe.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.x += strafe.x * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_camera.z += strafe.z * DISTANCE_TO_MOVE_KEYBOARD;
			element->m_looking.z += strafe.z * DISTANCE_TO_MOVE_KEYBOARD;
		}
	}

}

void SurfacePlotter::MouseMovement(int x, int y) {
	std::cout<<"button movement called"<<std::endl;
		std::cout<<"moving"<<std::endl;
		//we should check which surface the mouse is in
		for (auto element : m_surfaces) {
			vec3 looking_dir = Normalize(
					VectorSub(element->m_camera, element->m_looking));
			vec3 camera_dir = Normalize(
					CrossProduct(looking_dir, element->m_up));
			element->m_looking.x = element->m_looking.x
					- (x - m_mouseX) * DISTANCE_TO_MOVE_MOUSE * camera_dir.x;
			element->m_looking.y = element->m_looking.y
					+ (y - m_mouseY) * DISTANCE_TO_MOVE_MOUSE * camera_dir.y;
			element->m_looking.z = element->m_looking.z
					- (x - m_mouseX) * DISTANCE_TO_MOVE_MOUSE * camera_dir.z;
		}
	m_mouseX = x;
	m_mouseY = y;

}
void SurfacePlotter::MousePress(int button, int state, int x, int y) {
	std::cout<<"mouse pressed: "<<state<<std::endl;
	m_buttonReleased = state;
	m_mouseX = x;
	m_mouseY = y;

}

}

