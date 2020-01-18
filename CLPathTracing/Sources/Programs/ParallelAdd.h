#pragma once
#include "Core/Core.h"
#include "Renderer/CLProgram.h"

namespace PathTracer::Programs
{
	class ParallelAdd : public CLProgram
	{
	private:
		int numElements;

	public:
		ParallelAdd(CLContext* context, int n) : CLProgram(context, string("ParallelAdd"))
		{
			this->numElements = n;
		}

		float* Run(float* arrayA, float* arrayB)
		{
			float* result = new float[this->numElements]();
			Buffer clBufferA = Buffer(this->context->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numElements * sizeof(cl_int), arrayA);
			Buffer clBufferB = Buffer(this->context->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numElements * sizeof(cl_int), arrayB);
			Buffer clBufferOutput = Buffer(this->context->context, CL_MEM_WRITE_ONLY, numElements * sizeof(cl_int));

			kernel.setArg(0, clBufferA);
			kernel.setArg(1, clBufferB);
			kernel.setArg(2, clBufferOutput);

			CommandQueue queue = CommandQueue(context->context, context->renderer->RendererDevice);

			std::size_t globalWorkSize = numElements;
			std::size_t localWorkSize = 10;

			queue.enqueueNDRangeKernel(kernel, NULL, globalWorkSize, localWorkSize);
			queue.enqueueReadBuffer(clBufferOutput, CL_TRUE, 0, numElements * sizeof(cl_float), result);

			return result;
		}
	};
}