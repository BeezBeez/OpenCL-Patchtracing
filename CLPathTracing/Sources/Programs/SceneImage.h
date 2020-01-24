#pragma once
#include "Scenes/Scene.h"
#include "Renderer/CLProgram.h"

namespace PathTracer::Programs
{
	class SceneImage : public CLProgram
	{
	private:
		unsigned int ImageWidth, ImageHeight, ImageSamples, RayBounceCount;
		cl_float4* SampleOutput;
		Scenes::Sphere* cpuSpheres;
		std::size_t spheresCount;
		string ImageName;

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
		SceneImage(CLContext* context, Scenes::Scene* scene, string ImageName, unsigned int bounceCount = 2U, unsigned int samples = 32U, unsigned int width = 1280, unsigned int height = 720) : CLProgram(context, string("SceneImage"))
		{
			this->ImageWidth = width;
			this->ImageHeight = height;
			this->ImageSamples = samples;
			this->RayBounceCount = bounceCount;
			this->ImageName = ImageName;
			this->SampleOutput = new cl_float3[width * height];

			spheresCount = scene->GetSpheresCount();
			cpuSpheres = new Scenes::Sphere[spheresCount];
			cpuSpheres = scene->GetObjects();
		}

		void Run()
		{
			ConsoleOutput << "Sending " << spheresCount << " spheres to GPU...\n";
			Buffer clOutput = Buffer(context->context, CL_MEM_WRITE_ONLY, ImageWidth * ImageHeight * sizeof(cl_float3));
			Buffer clSceneSpheres = Buffer(context->context, CL_MEM_READ_ONLY, spheresCount * sizeof(Scenes::Sphere));
			context->queue.enqueueWriteBuffer(clSceneSpheres, CL_TRUE, 0, spheresCount * sizeof(Scenes::Sphere), cpuSpheres);

			kernel.setArg(0, clSceneSpheres);
			kernel.setArg(1, ImageWidth);
			kernel.setArg(2, ImageHeight);
			kernel.setArg(3, (int)spheresCount);
			kernel.setArg(4, RayBounceCount);
			kernel.setArg(5, ImageSamples);
			kernel.setArg(6, clOutput);

			SetGlobalWorkSize(ImageWidth * ImageHeight);

			ConsoleOutput << "Rendering started...\n";

			auto startTime = high_resolution_clock::now();

			context->queue.enqueueNDRangeKernel(kernel, 0, GetGlobalWorkSize(), GetLocalWorkSize());

			if (context->queue.finish() != CL_SUCCESS) {
				throw new exception("Image rendering failed !");
			}

			auto endTime = high_resolution_clock::now();
			auto renderTime = duration_cast<milliseconds>(endTime - startTime);
			ConsoleOutput << "Image rendering complete in " << renderTime.count() << "ms !\nWriting result to disk...\n";

			context->queue.enqueueReadBuffer(clOutput, CL_TRUE, 0, ImageWidth * ImageHeight * sizeof(cl_float3), SampleOutput);

			SaveImage();

			delete SampleOutput;
		}
	};
}