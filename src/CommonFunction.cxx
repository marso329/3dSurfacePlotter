

#include "CommonFunction.h"

namespace SurfacePlotting{

unsigned short getCacheLineSize(){
	std::ifstream cacheline("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size");
	unsigned short val=64;
	if (!cacheline.is_open()) {
			return val;
	}
	cacheline>>val;
	cacheline.close();
	return val;
}


namespace MATH {

double cos(double rad) {
	return std::cos(rad * PI / 180.0);
}

double sin(double rad) {
	return std::sin(rad * PI / 180.0);
}

double acos(double x) {
	return std::acos(x) * 180.0 / PI;
}

double asin(double x) {
	return std::asin(x) * 180.0 / PI;
}

static double atan(double z) {
	return std::atan(z) * 180.0 / PI;
}

static double degree_to_rad(double deg) {
	return deg / 180 * PI;
}
__attribute__((unused))
static double rad_to_degree(double rad) {
	return rad / PI * 180;
}

//return degree on unit cirle from x and y value
double degree(double x, double y) {
	double degree = MATH::atan(y / x);
	if (x > 0 && y > 0) {
		return degree;
	} else if ((x < 0 && y > 0) || (x < 0 && y < 0)) {
		return degree + 180;
	} else {
		return degree + 360;
	}
}

double distance(vec3 x, vec3 y) {
	return std::sqrt(
			std::pow(x.x - y.x, 2) + std::pow(x.y - y.y, 2)
					+ std::pow(x.z - y.z, 2));

}

}

/* report GL errors, if any, to stderr */
void printError(const char *functionName) {
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		if ((lastError != error) || (strcmp(functionName, lastErrorFunction))) {
			fprintf(stderr, "GL error 0x%X detected in %s\n", error,
					functionName);
			strcpy(lastErrorFunction, functionName);
			lastError = error;
		}
	}
}


void dumpInfo(void) {
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void printProgramInfoLog(GLuint obj, const char *vfn, const char *ffn,
		const char *gfn, const char *tcfn, const char *tefn) {
	GLint infologLength = 0;
	GLint charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 2) {
		if (gfn == NULL)
			fprintf(stderr, "[From %s+%s:]\n", vfn, ffn);
		else if (tcfn == NULL || tefn == NULL)
			fprintf(stderr, "[From %s+%s+%s:]\n", vfn, ffn, gfn);
		else
			fprintf(stderr, "[From %s+%s+%s+%s+%s:]\n", vfn, ffn, gfn, tcfn,
					tefn);
		infoLog = reinterpret_cast<char*>(malloc(infologLength));
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		fprintf(stderr, "%s\n", infoLog);
		free(infoLog);
	}
}

// Infolog: Show result of shader compilation
void printShaderInfoLog(GLuint obj, const char *fn) {
	GLint infologLength = 0;
	GLint charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 2) {
		fprintf(stderr, "[From %s:]\n", fn);
		infoLog = reinterpret_cast<char*>(malloc(infologLength));
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		fprintf(stderr, "%s\n", infoLog);
		free(infoLog);
	}
}

// Compile a shader, return reference to it
GLuint compileShaders(const char *vs, const char *fs, const char *gs,
		const char *tcs, const char *tes, const char *vfn, const char *ffn,
		const char *gfn, const char *tcfn, const char *tefn) {
	GLuint v = 0, f = 0, g = 0, tc = 0, te = 0, p = 0;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	glCompileShader(v);
	glCompileShader(f);
	if (gs != NULL) {
		g = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(g, 1, &gs, NULL);
		glCompileShader(g);
	}
#ifdef GL_TESS_CONTROL_SHADER
	if (tcs != NULL) {
		tc = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tc, 1, &tcs, NULL);
		glCompileShader(tc);
	}
	if (tes != NULL) {
		te = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(te, 1, &tes, NULL);
		glCompileShader(te);
	}
#endif
	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);
	if (gs != NULL)
		glAttachShader(p, g);
	if (tcs != NULL)
		glAttachShader(p, tc);
	if (tes != NULL)
		glAttachShader(p, te);
	glLinkProgram(p);
	glUseProgram(p);

	printShaderInfoLog(v, vfn);
	printShaderInfoLog(f, ffn);
	if (gs != NULL)
		printShaderInfoLog(g, gfn);
	if (tcs != NULL)
		printShaderInfoLog(tc, tcfn);
	if (tes != NULL)
		printShaderInfoLog(te, tefn);

	printProgramInfoLog(p, vfn, ffn, gfn, tcfn, tefn);

	return p;
}

char* readFile(const char *file) {
	FILE *fptr;
	long length;
	char *buf;

	if (file == NULL)
		return NULL;

	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = reinterpret_cast<char*>(malloc(length + 1)); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	__attribute__((unused))
	 size_t data_read = fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
}

GLuint loadShadersGT(const char *vertFileName, const char *fragFileName,
		const char *geomFileName, const char *tcFileName,
		const char *teFileName)
// With tesselation shader support
		{
	char *vs, *fs, *gs, *tcs, *tes;
	GLuint p = 0;

	vs = readFile(vertFileName);
	fs = readFile(fragFileName);
	gs = readFile(geomFileName);
	tcs = readFile(tcFileName);
	tes = readFile(teFileName);
	if (vs == NULL)
		fprintf(stderr, "Failed to read %s from disk.\n", vertFileName);
	if (fs == NULL)
		fprintf(stderr, "Failed to read %s from disk.\n", fragFileName);
	if ((gs == NULL) && (geomFileName != NULL))
		fprintf(stderr, "Failed to read %s from disk.\n", geomFileName);
	if ((tcs == NULL) && (tcFileName != NULL))
		fprintf(stderr, "Failed to read %s from disk.\n", tcFileName);
	if ((tes == NULL) && (teFileName != NULL))
		fprintf(stderr, "Failed to read %s from disk.\n", teFileName);
	if ((vs != NULL) && (fs != NULL))
		p = compileShaders(vs, fs, gs, tcs, tes, vertFileName, fragFileName,
				geomFileName, tcFileName, teFileName);
	if (vs != NULL)
		free(vs);
	if (fs != NULL)
		free(fs);
	if (gs != NULL)
		free(gs);
	if (tcs != NULL)
		free(tcs);
	if (tes != NULL)
		free(tes);
	return p;
}

GLuint loadShaders(const char *vertFileName, const char *fragFileName) {
	return loadShadersGT(vertFileName, fragFileName, NULL, NULL, NULL);
}


}
