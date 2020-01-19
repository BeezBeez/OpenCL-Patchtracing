#pragma once
#include "Core/Core.h"
#include "Renderer/CLProgram.h"

namespace PathTracer::Programs
{
	class SampleImage : public CLProgram
	{
	private:
		unsigned int ImageWidth, ImageHeight;
		cl_float4* SampleOutput;
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
		SampleImage(CLContext* context, string ImageName, unsigned int width = 1280, unsigned int height = 720) : CLProgram(context, string("SampleImage"))
		{
			this->ImageWidth = width;
			this->ImageHeight = height;
			this->ImageName = ImageName;
			this->SampleOutput = new cl_float3[width * height];
		}

		~SampleImage()
		{
			delete SampleOutput;
		}

		void Run(bool renderNormalBuffer = false)
		{
			Buffer clSampleOutput = Buffer(context->context, CL_MEM_WRITE_ONLY, ImageWidth * ImageHeight * sizeof(cl_float3));

			kernel.setArg(0, clSampleOutput);
			kernel.setArg(1, ImageWidth);
			kernel.setArg(2, ImageHeight);
			kernel.setArg(3, (int)renderNormalBuffer);

			std::size_t global_work_size = ImageWidth * ImageHeight;
			std::size_t local_work_size = 64;

			context->queue.enqueueNDRangeKernel(kernel, 0, global_work_size, local_work_size);

			//Blocks until all previously queued CL commands are issued and completed.
			if (context->queue.finish() != CL_SUCCESS)
			{
				throw new exception("Image ray-tracing failed !");
			}

			context->queue.enqueueReadBuffer(clSampleOutput, CL_TRUE, 0, ImageWidth * ImageHeight * sizeof(cl_float3), SampleOutput);
			ConsoleOutput << "Image rendering complete !\nWriting buffer to disk...\n";

			SaveImage();

			ConsoleOutput << "Rendering done !\nImage saved in \"./OutputImages/" + ImageName + ".ppm\"\n";
			WaitForEnter();
		}

		unsigned int GetImageWidth() { return this->ImageWidth; }
		unsigned int GetImageHeight() { return this->ImageHeight; }
	};
}