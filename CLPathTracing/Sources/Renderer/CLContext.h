#pragma once
#include "Core/Core.h"
#include "CLRenderer.h"

namespace PathTracer
{
	class CLContext
	{
	public:
		Context context;
		CLRenderer* renderer;

	public:
		CLContext(CLRenderer* renderer)
		{
			this->renderer = renderer;
			context = Context(renderer->RendererDevice);
		}
	};
}