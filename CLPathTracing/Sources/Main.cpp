#include "Core/Application.h"

Programs::SceneImage* SceneImageProgram;
GLRenderer* glRenderer;

const pair<string, pair<int, int>> Quality[9] = { make_pair("Very Low", make_pair(2,8)), make_pair("Low", make_pair(2,16)), make_pair("Medium", make_pair(2,32)), make_pair("High", make_pair(3,48)), make_pair("Very High", make_pair(3,64)), make_pair("Super high", make_pair(3, 96)), make_pair("Ultra", make_pair(4, 128)), make_pair("Excellent but cheep visuals", make_pair(2, 512)), make_pair("Perfection", make_pair(4, 2048)) };

string QualityName = "Very Low";
int Bounces = 0;
int Samples = 1;

void Update(int value) {
	glutPostRedisplay();
	glutTimerFunc(15, Update, 0);
}

void RenderFrame()
{
	SceneImageProgram->frameNumber++;

	SceneImageProgram->scenePtr->Update(SceneImageProgram->frameNumber);
	SceneImageProgram->context->queue.enqueueWriteBuffer(SceneImageProgram->clSceneSpheres, CL_TRUE, 0, SceneImageProgram->spheresCount * sizeof(Scenes::Sphere), SceneImageProgram->scenePtr->GetObjects());
	auto start = high_resolution_clock::now();

	SceneImageProgram->kernel.setArg(0, SceneImageProgram->clSceneSpheres);
	SceneImageProgram->kernel.setArg(7, SceneImageProgram->WangHash(SceneImageProgram->frameNumber));
	SceneImageProgram->RunKernel();

	glRenderer->Draw();

	auto end = high_resolution_clock::now();
	auto renderTime = duration_cast<milliseconds>(end - start);
	auto title = "OpenCL Viewport - Sphere Path tracer [" + to_string(renderTime.count()) + " ms / " + toStringWithPrecision(1.0f / (renderTime.count() / 1000.0f), 1) + " FPS] - " + QualityName + " " + std::to_string(Bounces) + " bounces | " + std::to_string(Samples) + " samples";
	if (SceneImageProgram->frameNumber % 5 == 0) {
		glutSetWindowTitle(title.c_str());
	}
}

void InitQuality()
{
	int QualityLevel = 1;
	ConsoleOutput << "Choose a renderer quality :" << endl;
	for (std::size_t i = 0; i < ARRAY_SIZE(Quality); i++)
	{
		ConsoleOutput << '\t' << std::to_string(i + 1) << " - " << Quality[i].first << " (" << Quality[i].second.first << " bounces / " << Quality[i].second.second << " samples)" << endl;
	}

	ConsoleOutput << endl << "Quality level : ";
	ConsoleInput >> QualityLevel;

	//Prevent invalid user input
	while (QualityLevel < 1 || QualityLevel > ARRAY_SIZE(Quality))
	{
		ConsoleInput.clear();
		ConsoleInput.ignore(ConsoleInput.rdbuf()->in_avail(), '\n');

		ConsoleOutput << "ERROR : Invalid quality level !\n";
		ConsoleOutput << endl << "Quality level : ";
		ConsoleInput >> QualityLevel;
	}

	QualityName = Quality[QualityLevel - 1].first;
	Bounces = Quality[QualityLevel - 1].second.first;
	Samples = Quality[QualityLevel - 1].second.second;
}

int main(int argc, char** argv)
{

	//Initialize OpenGL
	glRenderer = new GLRenderer(960, 540);
	glRenderer->InitRenderer(argc, argv);

	//Initialize OpenCL
	CLRenderer clRenderer = CLRenderer();
	CLContext context = CLContext(&clRenderer);

	glRenderer->GenerateVertexBuffer(&glRenderer->VertexBuffer);
	Update(0);

	glFinish(); //Make sure OpenGL is finished before we proceed

	glRenderer->SetRenderCallback(RenderFrame);

	InitQuality();
	SceneImageProgram = new Programs::SceneImage(&context, glRenderer->VertexBuffer, new Scenes::CornellBoxEcoPlus(), Bounces, Samples, glRenderer->ViewportWidth, glRenderer->ViewportHeight);
	SceneImageProgram->Run();

	glutMainLoop();
}