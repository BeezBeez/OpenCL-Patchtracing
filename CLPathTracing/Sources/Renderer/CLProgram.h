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
		std::size_t globalWorkSize = 0;
		std::size_t localWorkSize = 0;
		string KernelCode;
		string KernelName;

	private:
		bool CompileProgram()
		{
			//Create an OpenCL Program using runtime compilation
			program = Program(context->context, KernelCode);

			//Build the program and check for compilation errors
			cl_int result = program.build({ context->renderer->RendererDevice }, "");
			if (result != CL_SUCCESS)
			{
				ConsoleOutput << "Compilation error [ERR : " << result << "]" << endl;
				PrintBuildLog();

				return false;
			}
			else
			{
		    /*  Kernels are the basic units of executable code that run on the OpenCL Device.
				The kernel forms the starting points into the OpenCL program, analogous to main() in CPU code.  */

				kernel = Kernel(program, "main");

				//Apply a default local work size
				SetLocalWorkSize(kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(context->renderer->RendererDevice));
				ConsoleOutput << "Kernel compilation completed succesfully !\n";
				ConsoleOutput << "Kernel work group size : " << localWorkSize << endl;
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
			this->KernelName = "CustomKernel";
			this->KernelCode = KernelCode;

			if (!CompileProgram()) {
				ConsoleOutput << "ERROR : Program creation failed using custom kernel code !";
			}
		}

		CLProgram(CLContext* context, string KernelName)
		{
			this->KernelName = KernelName;
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

		void SetLocalWorkSize(std::size_t value)
		{
			localWorkSize = value;
		}

		void SetGlobalWorkSize(std::size_t value)
		{
			globalWorkSize = value;

			if (globalWorkSize % localWorkSize != 0) { //Ensure the global work size is a multiple of local work size
				SetGlobalWorkSize((globalWorkSize / localWorkSize + 1) * localWorkSize);
			}
		}

		std::size_t GetLocalWorkSize() { return localWorkSize; }
		std::size_t GetGlobalWorkSize() { return globalWorkSize; }

		void PrintKernelCode()
		{
			ConsoleOutput << KernelCode << endl;
		}

		void PrintBuildLog()
		{
			//Fetch the build log and print it to the console in ConsoleError buffer
			string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context->renderer->RendererDevice);
			ConsoleError << "Build log : " << endl << buildLog << endl << endl;

			//Print the error log to a file for further analysis
			ofstream logFile;
			logFile.open("./BuildLogs/ErrorLog-" + this->KernelName + ".log");
			logFile << "KERNEL COMPILATION BUILD LOG [" << this->KernelName << "]" << endl;
			logFile << buildLog << endl;
			ConsoleOutput << "Build log saved in \"./BuildLogs/ErrorLog-" << this->KernelName << ".log" << endl << endl;
			logFile.close();

			WaitForEnter();
		}
	};
}