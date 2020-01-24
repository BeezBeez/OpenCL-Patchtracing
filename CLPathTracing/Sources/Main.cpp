#include "Core/Application.h"

int main()
{
	CLRenderer Renderer = CLRenderer();

	Renderer.SelectPlatform();
	ConsoleOutput << "\nUsing OpenCL Platform : " << Renderer.RendererPlatform.getInfo<CL_PLATFORM_NAME>() << endl << endl;

	Renderer.SelectDevice();
	ConsoleOutput << "\nUsing OpenCL Device: " << Renderer.RendererDevice.getInfo<CL_DEVICE_NAME>() << endl << endl;
	Renderer.PrintDeviceInfo();

	CLContext context = CLContext(&Renderer);

	WaitForEnter();

	//const int numElements = 10;
	//Programs::ParallelAdd ParallelAddProgram = Programs::ParallelAdd(&context, numElements);

	//float cpuArrayA[numElements] = { 0.0f, 1.0f, 2.0f, 8.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
	//float cpuArrayB[numElements] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
	//float* cpuOutput = ParallelAddProgram.Run(cpuArrayA, cpuArrayB);

	////Print results to console
	//ConsoleOutput << "\nRESULTS :\n";
	//for (int i = 0; i < numElements; i++) {
	//	ConsoleOutput << cpuArrayA[i] << " + " << cpuArrayB[i] << " = " << cpuOutput[i] << endl;
	//}

	//WaitForEnter();

	//Programs::SampleImage SampleImageProgram = Programs::SampleImage(&context, "UVGradient");
	//SampleImageProgram.Run(false);

	Programs::SceneImage SceneImageProgram = Programs::SceneImage(&context, new Scenes::CornellBoxEcoPlus(), "CornellBoxEcoPlus-Test", 5, 4096);
	SceneImageProgram.Run();

	WaitForEnter();
}