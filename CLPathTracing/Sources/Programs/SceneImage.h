#pragma once
#include "Scenes/Scene.h"
#include "Renderer/CLProgram.h"

namespace PathTracer::Programs
{
	class SceneImage : public CLProgram
	{
	private:
		unsigned int ImageWidth, ImageHeight;
		cl_float4* SampleOutput;
		Scenes::Sphere* cpuSpheres;
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
		SceneImage(CLContext* context, Scenes::Scene* scene, string ImageName, unsigned int width = 1280, unsigned int height = 720) : CLProgram(context, string("SceneImage"))
		{
			this->ImageWidth = width;
			this->ImageHeight = height;
			this->ImageName = ImageName;

			cpuSpheres = new Scenes::Sphere[];
			cpuSpheres = scene->GetObjects();
		}

		void Run()
		{

		}
	};
}