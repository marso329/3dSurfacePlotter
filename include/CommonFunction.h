/*!
   \file    CommonFunction.h
   \author  msoderen
   \date    Aug 10, 2018
   \version 0.0.1

   \brief Header-file for the Template class.

   \section DESCRIPTION
	
	Detailed description on this file. 

 */


#ifndef INCLUDE_COMMONFUNCTION_H_
#define INCLUDE_COMMONFUNCTION_H_

//System packages
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <chrono>

#define GL_GLEXT_PROTOTYPES
//extra packages
#include <GL/freeglut.h>

//intra-packages
#include "VectorUtils3.h"

namespace SurfacePlotting {
static GLenum lastError = 0;
static char lastErrorFunction[1024] = "";

unsigned short getCacheLineSize();
namespace MATH{
#define PI 3.14159265
double cos(double rad);
double sin(double rad);
double acos(double x);
double asin(double x);
static double atan(double z);
static double degree_to_rad(double deg);
static double rad_to_degree(double rad);
double degree(double x, double y);
double distance(vec3 x, vec3 y);
}

void printError(const char *functionName);
void dumpInfo(void);
void printProgramInfoLog(GLuint obj, const char *vfn, const char *ffn,
		const char *gfn, const char *tcfn, const char *tefn);
void printShaderInfoLog(GLuint obj, const char *fn);
GLuint compileShaders(const char *vs, const char *fs, const char *gs,
		const char *tcs, const char *tes, const char *vfn, const char *ffn,
		const char *gfn, const char *tcfn, const char *tefn);
char* readFile(const char *file);
GLuint loadShadersGT(const char *vertFileName, const char *fragFileName,
		const char *geomFileName, const char *tcFileName,
		const char *teFileName);
GLuint loadShaders(const char *vertFileName, const char *fragFileName);


}

#endif
