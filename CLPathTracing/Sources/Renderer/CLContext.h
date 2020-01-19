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
		CommandQueue queue;

	public:
		CLContext(CLRenderer* renderer)
		{
			this->renderer = renderer;
			context = Context(renderer->RendererDevice);
			queue = CommandQueue(context, renderer->RendererDevice);
		}
	};
}