#pragma once
#include "Scenes/Scene.h"
#include "Renderer/OpenCL/CLProgram.h"

namespace PathTracer::Programs
{
	class SceneImage : public CLProgram
	{
	private:
		unsigned int ImageWidth, ImageHeight, ImageSamples, RayBounceCount;
		cl_float4* SampleOutput;
		Scenes::Sphere* cpuSpheres;
		string ImageName;
		GLuint vertexBuffer;

		BufferGL clVertexBuffer;
		vector<Memory> clVertexBuffers;

	public:
		unsigned int frameNumber = 0;
		Buffer clSceneSpheres;
		Scenes::Scene* scenePtr;
		std::size_t spheresCount;

	private:
		inline float clamp(float x) { return x < .0f ? .0f : x > 1.f ? 1.f : x; } //Clamp a float value between 0 and 1
		inline int toInt(float x) { return int(clamp(x) * 255 + .5); } //Convert RGB float [0..1] to int [0..255]. We add 0.5 while converting value to always round it to the upper int value (Example int(254 + 0.5) = 255 instead of 254)

		void SaveImage()
		{
			//Write image to a PPM file
			ofstream resultImage;
			resultImage.open("./OutputImages/" + ImageName + ".ppm");
			resultImage << "P3\n" << ImageWidth << " " << ImageHeight << endl << 255 << endl;

			//Read RGB Values from kernel result
			for (unsigned int i = 0; i < ImageWidth * ImageHeight; i++) {
				float progression = ((float)i / ((float)ImageWidth * (float)ImageHeight)) * 100.f;
				if (fmodf(progression, 10) == .0f) {
					ConsoleOutput << "Progression : " << progression << "%" << endl;
				}
				resultImage << toInt(SampleOutput[i].s[0]) << " " << toInt(SampleOutput[i].s[1]) << " " << toInt(SampleOutput[i].s[2]) << " ";
			}

			resultImage.close();
		}
	public:
		SceneImage(CLContext* context, GLuint vbo, Scenes::Scene* scene, unsigned int bounceCount = 2U, unsigned int samples = 32U, unsigned int width = 1280, unsigned int height = 720) : CLProgram(context, string("SceneImage"))
		{
			this->ImageWidth = width;
			this->ImageHeight = height;
			this->ImageSamples = samples;
			this->RayBounceCount = bounceCount;
			this->ImageName = ImageName;
			this->SampleOutput = new cl_float3[width * height];
			this->vertexBuffer = vbo;
			this->scenePtr = scene;

			spheresCount = scenePtr->GetSpheresCount();
			cpuSpheres = new Scenes::Sphere[spheresCount];
			cpuSpheres = scenePtr->GetObjects();
		}

		unsigned int GetImageWidth() { return ImageWidth; }
		unsigned int GetImageHeight() { return ImageHeight; }

		//Hash function to calculate new seed for each frame
		unsigned int WangHash(unsigned int seed)
		{
			seed = (seed ^ 61) ^ (seed >> 16);
			seed *= 9;
			seed = seed ^ (seed >> 4);
			seed *= 0x27d4eb2d;
			seed = seed ^ (seed >> 15);
			return seed;
		}

		void Run()
		{
			ConsoleOutput << "Sending " << spheresCount << " spheres to GPU...\n";
			//Buffer clOutput = Buffer(context->context, CL_MEM_WRITE_ONLY, ImageWidth * ImageHeight * sizeof(cl_float3));

			clSceneSpheres = Buffer(context->context, CL_MEM_READ_ONLY, spheresCount * sizeof(Scenes::Sphere));
			context->queue.enqueueWriteBuffer(clSceneSpheres, CL_TRUE, 0, spheresCount * sizeof(Scenes::Sphere), cpuSpheres);

			clVertexBuffer = BufferGL(context->context, CL_MEM_WRITE_ONLY, this->vertexBuffer);

			clVertexBuffers.push_back(clVertexBuffer);

			kernel.setArg(0, clSceneSpheres);
			kernel.setArg(1, ImageWidth);
			kernel.setArg(2, ImageHeight);
			kernel.setArg(3, (int)spheresCount);
			kernel.setArg(4, RayBounceCount);
			kernel.setArg(5, ImageSamples);
			kernel.setArg(6, clVertexBuffer);
			kernel.setArg(7, frameNumber);
		}

		void RunKernel()
		{
			SetGlobalWorkSize(ImageWidth * ImageHeight);
			glFinish();

			//Passes all the Vexter Buffer Objects
			context->queue.enqueueAcquireGLObjects(&clVertexBuffers);
			context->queue.finish();

			//Start the kernel
			context->queue.enqueueNDRangeKernel(kernel, 0, GetGlobalWorkSize(), GetLocalWorkSize());
			context->queue.finish();

			//Release the vertex buffer objects to let OpenGL manipulate them
			context->queue.enqueueReleaseGLObjects(&clVertexBuffers);
			context->queue.finish();
		}
	};
}