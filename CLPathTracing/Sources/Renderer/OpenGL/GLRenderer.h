#pragma once
#include "Core/Core.h"

namespace PathTracer
{
	class GLRenderer
	{
	public:
		GLuint VertexBuffer;
		unsigned int ViewportWidth, ViewportHeight;

	public:
		GLRenderer(unsigned int width, unsigned int height)
		{
			this->ViewportWidth = width;
			this->ViewportHeight = height;
		}

		void SetRenderCallback(void(*RenderCallback)())
		{
			glutDisplayFunc(RenderCallback);
		}

		void InitRenderer(int argc, char** argv)
		{
			glutInit(&argc, argv); //Initialize GLUT for OpenGL Viewport	
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //Use RGB display mode with Double-Buffering

			glutInitWindowPosition(80, 80); //Define initial window position on screen
			glutInitWindowSize(ViewportWidth, ViewportHeight);
			glutCreateWindow("OpenCL Viewport - Sphere Path tracer");

			//Load OpenGL Extensions
			glewInit();

			//Initialize OpenGL
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glMatrixMode(GL_PROJECTION);
			gluOrtho2D(0.0, ViewportWidth, ViewportHeight, 0.0);

			ConsoleOutput << "INFO : OpenGL Initialized !\n";
		}

		void GenerateVertexBuffer(GLuint* vbo)
		{
			glGenBuffers(1, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, *vbo);

			unsigned int size = ViewportWidth * ViewportHeight * sizeof(cl_float3);
			glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void Draw()
		{
			//Clear all pixels then render from the vertex buffer object
			glClear(GL_COLOR_BUFFER_BIT);
			glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
			glVertexPointer(2, GL_FLOAT, 16, 0);
			glColorPointer(4, GL_UNSIGNED_BYTE, 16, (GLvoid*)8);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glDrawArrays(GL_POINTS, 0, ViewportWidth * ViewportHeight); //Draw a "point cloud" of pixels

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			glutSwapBuffers(); //Flip BackBuffer to Screen
		}


	};
}