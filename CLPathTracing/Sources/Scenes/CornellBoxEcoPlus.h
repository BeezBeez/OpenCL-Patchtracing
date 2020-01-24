#pragma once
#include "Scene.h"

namespace PathTracer::Scenes
{
	class CornellBoxEcoPlus : public Scene
	{
	public:
		CornellBoxEcoPlus() : Scene()
		{
			sphereObjects =
			{
				//Left wall
				Sphere(200.f, float3(-200.6f, .0f, 0.0f), float3(1.f, .15f, .15f)),

				//Right wall
				Sphere(200.f, float3(200.6f, .0f, 0.0f), float3(0.f, 1.0f, .15f)),

				//Floor
				Sphere(200.f, float3(0.0f, -200.4f, 0.0f), float3(0.9f, 0.9f, 0.9f)),

				//Ceiling
				Sphere(200.f, float3(0.0f, 200.4f, 0.0f), float3(0.9f, 0.9f, 0.9f)),

				//Back wall
				Sphere(200.f, float3(0.0f, 0.0f, -200.4f), float3(0.9f, 0.9f, 0.9f)),

				//Spheres on ground
				Sphere(0.16f, float3(-0.25f, -0.24f, -0.1f), float3(0.9f, 0.8f, 0.7f)),
				Sphere(0.16f, float3(0.25f, -0.24f, -0.1f), float3(0.9f, 0.8f, 0.7f)),

				//Light source (Sphere light)
				Sphere(1.05f, float3(0.0f, 1.39f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(3.0f, 3.0f, 3.0f))
			};
		}

		virtual void Update(int frameNumber) override
		{
			sphereObjects[5].Position.s[1] = (cosf(frameNumber * 0.3) * 0.15) - 0.24f;
			sphereObjects[6].Position.s[1] = (cosf(frameNumber * 0.3) * -0.15) - 0.24f;
		}
	};
}