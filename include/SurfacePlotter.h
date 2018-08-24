/*!
   \file    3dSurfacePlotter.h
   \author  msoderen
   \date    Aug 10, 2018
   \version 0.0.1

   \brief Header-file for the 3dSurfacePlotter class.

   \section DESCRIPTION
	
	Detailed description on this file. 

 */


#ifndef INCLUDE_SURFACEPLOTTER_H_
#define INCLUDE_SURFACEPLOTTER_H_

#define GL_GLEXT_PROTOTYPES
//System packages
#include <vector>
#include <chrono>

//extra packages
#include <GL/freeglut.h>

//intra-packages
#include "Surface.h"
#include "VectorUtils3.h"

namespace SurfacePlotting{
class SurfaceBase;
template<class T> class Surface;


/*! \class 3dSurfacePlotter
    \brief The 3dSurfacePlotter class which is used for 3dSurfacePlotter.

	More info on the 3dSurfacePlotter class
*/
class SurfacePlotter {
public:
	/*!
	 * \fn 3dSurfacePlotter()
	 * \brief constructor for 3dSurfacePlotter class.
	 */
	SurfacePlotter();
	/*!
	 * \fn ~3dSurfacePlotter()
	 * \brief destructor for 3dSurfacePlotter class.
	 */
	~SurfacePlotter();

	template<typename T>
	Surface<T>* addSurface(const std::size_t& rows,const std::size_t& cols){
		Surface<T>* temp=new Surface<T>(rows,cols);
		m_surfaces.push_back(temp);
		return temp;
	}
	void show(const std::size_t& width=640,const std::size_t& height=480);
	void Draw();

	void Timer(int i);

	void Keyboard(unsigned char key, int x_disp, int y_disp);

	void KeyboardSpecial(int key, int x_disp, int y_disp);

	void MouseMovement(int x,int y);

	void MousePress(int button, int state, int x,int y);

protected:

private:
	/*!
	 * \fn 3dSurfacePlotter(const 3dSurfacePlotter& )
	 * \brief Deleted copy constructor.
	 */
	SurfacePlotter(const SurfacePlotter& ) {};
	/*!
	 * \fn 3dSurfacePlotter operator=(const 3dSurfacePlotter& server)
	 * \brief Deleted copy assignment.
	 */
	SurfacePlotter operator=(const SurfacePlotter& server) {return server;}

	void initOpenGl();


	std::vector<SurfaceBase*> m_surfaces;

	std::size_t m_width=0;

	std::size_t m_height=0;

	int m_maxTextureSize=0;

	std::size_t timeLastFrame=0;
	//used to keep track on if the mouse button is pressed or released
	bool m_buttonReleased = true;
	//used to store the mouse position from last iteration to know how much to move the looking_pos
	int m_mouseX = 0;
	int m_mouseY = 0;
	float DISTANCE_TO_MOVE_MOUSE=0.05;
	float DISTANCE_TO_MOVE_KEYBOARD=0.09;
};

//all of this is because of FreeGlut crap
static SurfacePlotter* plotter;


}

#endif
