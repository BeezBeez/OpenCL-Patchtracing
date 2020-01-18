#pragma once
#include "Core/Core.h"
#include "CLContext.h"

namespace PathTracer
{
	class CLProgram
	{
	public:
		Program program;
		CLContext* context;
		Kernel kernel;

	private:
		string KernelCode;

	private:
		bool CompileProgram()
		{
			//Create an OpenCL Program using runtime compilation
			program = Program(context->context, KernelCode);
			ConsoleOutput << endl << KernelCode << endl;
			//Build the program and check for compilation errors
			cl_int result = program.build({ context->renderer->RendererDevice }, "");
			if (result != CL_SUCCESS)
			{
				ConsoleOutput << "Compilation error [ERR : " << result << "]" << endl;

				string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context->renderer->RendererDevice);
				ConsoleError << "Build log : " << endl << buildLog << endl;
				return false;
			}
			else
			{
				/*
				Kernels are the basic units of executable code that run on the OpenCL Device.
				The kernel forms the starting points into the OpenCL program, analogous to main() in CPU code.
				*/
				kernel = Kernel(program, "main");
				ConsoleOutput << "Kernel compilation completed succesfully !";
				return true;
			}
		}

	public:
		CLProgram()
		{
			context = nullptr;
			throw new exception("Program can't be initialized without context !");
		}

		CLProgram(CLContext* context, const char* KernelCode)
		{
			this->context = context;
			this->KernelCode = KernelCode;

			if (!CompileProgram()) {
				ConsoleOutput << "ERROR : Program creation failed using custom kernel code !";
			}
		}

		CLProgram(CLContext* context, string KernelName)
		{
			this->context = context;
			ifstream kernelFile("./Kernels/" + KernelName + ".cl");
			if (!kernelFile.is_open())
			{
				throw new exception("Kernel file not found !");
			}

			stringstream buffer;
			buffer << kernelFile.rdbuf();

			KernelCode = buffer.str();

			if (!CompileProgram()) {
				ConsoleOutput << "ERROR : Program creation failed using \"" << KernelName << "\" !";
			}
		}

		void PrintKernelCode()
		{
			ConsoleOutput << KernelCode;
		}
	};
}