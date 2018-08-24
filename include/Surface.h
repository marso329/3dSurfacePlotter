/*!
 \file    Surface.h
 \author  msoderen
 \date    Aug 10, 2018
 \version 0.0.1

 \brief Header-file for the Surface class.

 \section DESCRIPTION

 Detailed description on this file.

 */

#ifndef INCLUDE_SURFACE_H_
#define INCLUDE_SURFACE_H_

#define GL_GLEXT_PROTOTYPES
#define UP 1
#define DOWN -1
//System packages
#include <cstddef>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <atomic>

//extra packages
#include <GL/freeglut.h>

//intra-packages
#include "SurfacePlotter.h"
#include "CommonFunction.h"

namespace SurfacePlotting {

class Tained {
public:
	std::size_t row = 0;
	std::size_t elements = 0;
	std::size_t offset = 0;
	float textureOffsetShift = 0.0;

};

class SurfaceBase {
	friend class SurfacePlotter;
public:
protected:

	/*!
	 * \fn Surface()
	 * \brief constructor for Surface class.
	 */
	SurfaceBase(const std::size_t& rows, const std::size_t& cols) :
			m_rows(rows), m_cols(cols) {
		m_textureData = reinterpret_cast<GLshort*>(aligned_alloc(
				getCacheLineSize(), m_cols * m_rows * sizeof(GLshort)));
	}
	;
	/*!
	 * \fn ~Surface()
	 * \brief destructor for Surface class.
	 */
	virtual ~SurfaceBase() {
	}
	;
	virtual void init()=0;
	virtual bool update()=0;
	bool updateRequired(){
		return m_tainted.size()!=0;
	}
	const std::size_t m_rows = 0;
	const std::size_t m_cols = 0;
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_tex = 0;

	vec3 m_camera;
	//stores att which position we are looking
	vec3 m_looking;
	//always 0,1,0
	vec3 m_up;

	mat4 m_frustum_matrix;

	int m_shaderProgram = 0;

	//std::size_t m_currentRow = 0;
	std::size_t m_oldestRow=0;

	GLfloat m_offset = 0;
	std::vector<Tained> m_tainted;
	std::atomic<bool> m_modifying = false;

	//for transfering to the texture memory
	GLshort* m_textureData=0;

	float m_pixelsPerRow=1.0;

	float m_pixelsPerColumn=1.0;

	bool m_firstShift=true;

private:

	/*!
	 * \fn Surface(const Surface& )
	 * \brief Deleted copy constructor.
	 */
	//SurfaceBase(const SurfaceBase& ) {};
	/*!
	 * \fn Surface operator=(const Surface& server)
	 * \brief Deleted copy assignment.
	 */
	//SurfaceBase operator=(const SurfaceBase& server) {return server;}
};

/*! \class Surface
 \brief The Surface class which is used for Surface.

 More info on the Surface class
 */
template<typename T>
class Surface: public SurfaceBase {
	friend class SurfacePlotter;
public:
	//row major
	bool addData(const T* data, const std::size_t& rowOffset = 0,
			const std::size_t& colsOffset = 0, const std::size_t& rows = 0,
			const std::size_t& cols = 0) {
		//simplest case
		if (rows == 0 && cols == 0) {
			memcpy(m_data, data, m_rows * m_cols * sizeof(T));
			return true;
		}
		return false;
	}
	bool addRow(const T* data,
			const int& shift = 0) {
		//if we are uploading data to the GPU or another thread is modifying the data
		bool expected = false;
		while (!std::atomic_compare_exchange_weak(&m_modifying, &expected, true)) {

		}
		memcpy((m_data + m_oldestRow * m_cols), data, m_cols * sizeof(T));
		for(std::size_t i=0;i<m_cols;i++){
			*(m_data + m_oldestRow * m_cols+i)=(*(m_data + m_oldestRow * m_cols+i)+m_offset)*m_yscale;
		}

		//Which part to update in the texture
		Tained temp;
		if(!m_firstShift){
		if (shift == UP) {
			temp.textureOffsetShift -= 1.0 / static_cast<float>(m_rows);
		} else if (shift == DOWN) {
			temp.textureOffsetShift += 1.0 / static_cast<float>(m_rows);
		}
		}
		m_firstShift=false;
		temp.elements = m_cols;
		temp.row = m_oldestRow;
		m_tainted.push_back(temp);
		m_oldestRow+=1;
		m_oldestRow%=m_rows;
		m_modifying = false;
		return true;
	}
	void setYOffset(T value){
		m_yoffset=value;
	}

	void setYScale(T value){
		m_yscale=value;
	}

	void setPixelsPerRow(const float& pixelsPerRow){
		assert(pixelsPerRow>0.0);
		m_pixelsPerRow=pixelsPerRow;
	}
	void setPixelsPerColumn(const float& pixelsPerColumn){
		assert(pixelsPerColumn>0.0);
		m_pixelsPerColumn=pixelsPerColumn;
	}

protected:
	virtual bool update() {
		//we dont have time to wait for the user to update the data, we take it next turn
		bool expected = false;
		if (!std::atomic_compare_exchange_weak(&m_modifying, &expected, true)) {
			return false;
		}
		glBindTexture(GL_TEXTURE_2D, m_tex);
		printError("update: binding texture");
		bool somethingToUpdate=false;


		for(auto element:m_tainted){
			for (std::size_t i = 0; i < element.elements; i++) {
				*(m_textureData + i) = static_cast<GLshort>(*(m_data+element.row*m_cols+element.offset + i));
			}
			glTexSubImage2D(GL_TEXTURE_2D, //target
					0, //level
					0, //xoffset
					element.row, //yoffset
					element.elements, //width
					1, //height
					GL_RED, //input format
					GL_SHORT, //input type
					m_textureData //data
					);
			int w, h;
			int miplevel = 0;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
			printError("update: glTexSubImage");
			m_offset+=element.textureOffsetShift;
			//m_offset=float(element.row) / float(m_rows);
			if(m_offset>1.0-1.0/m_rows){
				m_offset=0.0;
			}
			else if(m_offset<0){
				m_offset=1.0;
			}
			somethingToUpdate=true;
		}
		m_tainted.clear();
		m_modifying = false;
		return somethingToUpdate;
	}
	virtual void init() {
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		GLfloat* vertexDataBunches = reinterpret_cast<GLfloat*>(aligned_alloc(
				getCacheLineSize(), m_rows * m_cols * 5 * sizeof(GLfloat)));

		for (std::size_t i = 0; i < m_rows; i++) {
			for (std::size_t j = 0; j < m_cols; j++) {
				GLfloat x = static_cast<GLfloat>(j)*m_pixelsPerColumn;
				GLfloat y = 0.0f;
				GLfloat z = static_cast<GLfloat>(i)*m_pixelsPerRow;
				GLfloat texX = j / float(m_cols);
				GLfloat texY = i / float(m_rows);
				*(vertexDataBunches + (i * m_cols + j) * 5 + 0) = x;
				*(vertexDataBunches + (i * m_cols + j) * 5 + 1) = y;
				*(vertexDataBunches + (i * m_cols + j) * 5 + 2) = z;
				*(vertexDataBunches + (i * m_cols + j) * 5 + 3) = texX;
				*(vertexDataBunches + (i * m_cols + j) * 5 + 4) = texY;

			}
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_rows * m_cols * 5,
				vertexDataBunches,
				GL_STATIC_DRAW);

		// set up generic attrib pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
				reinterpret_cast<char*>( 0) + 0 * sizeof(GLfloat));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
				reinterpret_cast< char*>(0) + 3 * sizeof(GLfloat));

		printError("While setting up vertex and texture index buffer");

		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		GLuint* indexes = reinterpret_cast<GLuint*>(aligned_alloc(
				getCacheLineSize(),
				2 * (m_cols - 1) * (m_rows - 1) * 6 * sizeof(GLuint)));

		for (std::size_t i = 0; i < m_rows - 1; i++) {
			for (std::size_t j = 0; j < m_cols - 1; j++) {
				//first triangle
				indexes[(i * (m_cols - 1) + j) * 6] = static_cast<GLuint>(j
						+ m_cols * i);
				indexes[(i * (m_cols - 1) + j) * 6 + 1] = static_cast<GLuint>(j
						+ m_cols * (i + 1));
				indexes[(i * (m_cols - 1) + j) * 6 + 2] = static_cast<GLuint>(j
						+ m_cols * (i + 1) + 1);

				//second triangle
				indexes[(i * (m_cols - 1) + j) * 6 + 3] = static_cast<GLuint>(j
						+ m_cols * i);
				indexes[(i * (m_cols - 1) + j) * 6 + 4] = static_cast<GLuint>(j
						+ m_cols * (i + 1) + 1);
				indexes[(i * (m_cols - 1) + j) * 6 + 5] = static_cast<GLuint>(j
						+ m_cols * i + 1);

			}
		}

		// fill with data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(GLuint) * 6 * (m_cols - 1) * (m_rows - 1), indexes,
				GL_STATIC_DRAW);

		printError("While setting up index buffer");

		glGenTextures(1, &m_tex);

		glBindTexture(GL_TEXTURE_2D, m_tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (std::size_t i = 0; i < m_cols * m_rows; i++) {
			*(m_textureData + i) = static_cast<GLshort>(*(m_data + i));
		}
		glTexImage2D(GL_PROXY_TEXTURE_2D,     // Type of texture
				0,       // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				m_cols, // Image width  i.e. 640 for Kinect in standard mode
				m_rows, // Image height i.e. 480 for Kinect in standard mode
				0,              // Border width in pixels (can either be 1 or 0)
				GL_RED, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_SHORT,  // Image data type
				m_textureData);
		GLint format;
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
		GL_TEXTURE_INTERNAL_FORMAT, &format);
		if (format == 0) {
			throw std::runtime_error("Texture is not in a good shape");
		}
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
				0,       // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				m_cols, // Image width  i.e. 640 for Kinect in standard mode
				m_rows, // Image height i.e. 480 for Kinect in standard mode
				0,              // Border width in pixels (can either be 1 or 0)
				GL_RED, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_SHORT,  // Image data type
				m_textureData);        // The actual image data itself
		printError("While setting up texture");
		free(indexes);
		free(vertexDataBunches);
		//projection and modelview matrix
		m_camera = vec3(-2, 1, -2);
		m_looking = vec3(0.33, 0.33, 0.33);
		m_frustum_matrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.1, 50.0);
		m_up = vec3(0, 1, 0);

		m_shaderProgram = loadShaders("shaders/line.vert", "shaders/line.frag");
	}

private:
	/*!
	 * \fn Surface()
	 * \brief constructor for Surface class.
	 */
	Surface(const std::size_t& rows, const std::size_t& cols) :
			SurfaceBase(rows, cols) {
		assert(rows > 0 && cols > 0);
		//allocate enough aligned data to store all data
		m_data = reinterpret_cast<T*>(aligned_alloc(getCacheLineSize(),
				rows * cols * sizeof(T)));
	}
	;
	/*!
	 * \fn ~Surface()
	 * \brief destructor for Surface class.
	 */
	~Surface() {
	}
	;
	/*!
	 * \fn Surface(const Surface& )
	 * \brief Deleted copy constructor.
	 */
	Surface(const Surface&) {
	}
	;
	/*!
	 * \fn Surface operator=(const Surface& server)
	 * \brief Deleted copy assignment.
	 */
	Surface<T> operator=(const Surface<T>& server) {
		return server;
	}

	T* m_data = 0;
	T m_yoffset=0;
	T m_yscale=1;

};
}

#endif
