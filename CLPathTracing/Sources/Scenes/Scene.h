#pragma once
#include "Core/Core.h"

#define float3(x, y, z) {{x, y, z}}

namespace PathTracer::Scenes
{
	//Sphere object
	struct Sphere
	{
		cl_float Radius;
		//Dummy variables are required for a safe memory alignment
		cl_float dummy1;
		cl_float dummy2;
		cl_float dummy3;
		cl_float3 Position;
		cl_float3 Color;
		cl_float3 Emission;

		Sphere() { }

		Sphere(cl_float radius, cl_float3 position, cl_float3 color, cl_float3 emission = float3(.0f, .0f, .0f))
		{
			this->Radius = radius;
			this->Position = position;
			this->Color = color;
			this->Emission = emission;

			this->dummy1 = .0f;
			this->dummy2 = .0f;
			this->dummy3 = .0f;
		}
	};

	class Scene
	{
	protected:
		std::vector<Sphere> sphereObjects;

	public:
		Scene()
		{
			sphereObjects = {};
		}

		Scene(std::vector<Sphere> objects)
		{
			sphereObjects = objects;
		}

		Sphere* GetObjects() { return sphereObjects.data(); }
		std::size_t GetSpheresCount() { return sphereObjects.size(); }
	};
}