#include "Core/Application.h"

Programs::SceneImage* SceneImageProgram;
GLRenderer* glRenderer;

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
	auto title = "OpenCL Viewport - Sphere Path tracer [" + to_string(renderTime.count()) + " ms / " + toStringWithPrecision(1.0f / (renderTime.count() / 1000.0f), 1) + " FPS]";
	if (SceneImageProgram->frameNumber % 5 == 0) {
		glutSetWindowTitle(title.c_str());
	}
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

	SceneImageProgram = new Programs::SceneImage(&context, glRenderer->VertexBuffer, new Scenes::CornellBoxEcoPlus(), 3, 32, glRenderer->ViewportWidth, glRenderer->ViewportHeight);
	SceneImageProgram->Run();

	glutMainLoop();
}