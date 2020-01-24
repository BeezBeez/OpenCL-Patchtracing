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
			this->renderer->InitRenderer();

			cl_context_properties ctxProperties[] = {
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
				CL_CONTEXT_PLATFORM, (cl_context_properties)this->renderer->RendererPlatform(),
				0
			};

			context = Context(renderer->RendererDevice, ctxProperties);
			queue = CommandQueue(context, renderer->RendererDevice);
		}
	};
}