#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include <iostream>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"

#include <entt/entt.hpp>

#include "Axiom/Render/DX12/D3D12Core.hpp"

#include <Axiom/Core/Allocator.hpp>
#include <Axiom/Core/Memory.hpp>
#include <Axiom/Core/BlockAllocator.hpp>
#include <Axiom/Render/DX12/D3D12Context.hpp>

#include <fstream>

static D3D12Context* context;
static IShader* vertexShader;
static IShader* fragmentShader;
static IBuffer* vertexBuffer, *indexBuffer;
static ICommandAllocator* commandAllocator;
static ICommandList* commandList;
static ICommandQueue* commandQueue;

inline char* ReadAllFile(const char* fileName, int* numCharacters = 0)
{
	FILE* fp = fopen(fileName, "r");
	if (!fp) {
		printf("file read failed");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	rewind(fp);
	// BOM check
	unsigned char test[3] = { 0 };
	fread(test, 1, 3, fp);

	if (test[0] != 0xEF || test[1] != 0xBB || test[2] != 0xBF) {
		rewind(fp);
	}

	char* fcontent = (char*)calloc(fsize + 1, 1);
	fread(fcontent, 1, fsize, fp);
	fclose(fp);

	if (numCharacters) *numCharacters = fsize;
	return fcontent;
}

void EditorApplication::OnInit()
{
	GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = UINew(UIWindow)
									.AutoCenter(true)
									.Size(Vector2i(1270, 720))
									.Title("Axiom");

	GEngine->Get<WindowManager>()->AddWindow(window, true);
#ifdef AX_WIN32
	SharedPtr<INativeWindow> nativeWindow = window->GetNativeWindow();
	context = new D3D12Context();
	context->Initialize(nativeWindow);
	char* shaderCode = ReadAllFile("../EngineAssets/Shaders/PBR.hlsl");
	vertexShader = context->CreateShader(shaderCode, "VS", EShaderType::Vertex);
	
	fragmentShader = context->CreateShader(shaderCode, "PS", EShaderType::Fragment);

	free(shaderCode);

	commandAllocator = context->CreateCommandAllocator(ECommandListType::Direct);
	// todo create for each thread
	commandList = context->CreateCommandList(commandAllocator, ECommandListType::Direct);

	commandQueue = context->CreateCommandQueue(ECommandListType::Direct, ECommandQueuePriority::Normal);

	struct PBRVertex {
		Vector3f position;
		Vector4 color;
	};

	static PBRVertex vertices[4] =
	{
		{ {-0.5f,-0.5f, 0.25f}, { 1.0f, 0.0f, 0.0f, 1.0f} },
		{ {-0.5f, 0.5f, 0.25f}, { 0.0f, 1.0f, 0.0f, 1.0f} },
		{ { 0.5f,-0.5f, 0.25f}, { 0.0f, 0.0f, 1.0f, 1.0f} },
		{ { 0.5f, 0.5f, 0.25f}, { 0.0f, 0.0f, 1.0f, 1.0f} }
	};

	static uint32 indices[6]{
		0, 1, 2,
		1, 3, 2
	};

	BufferDesc bufferDesc{};
	bufferDesc.Data = vertices;
	bufferDesc.ResourceUsage = EResourceUsage::VertexBuffer;
	bufferDesc.ElementByteStride = sizeof(PBRVertex);
	bufferDesc.Size = sizeof(PBRVertex) * _countof(vertices);

	vertexBuffer = context->CreateBuffer(bufferDesc, commandList);

	bufferDesc.Data = indices;
	bufferDesc.ResourceUsage = EResourceUsage::IndexBuffer;
	bufferDesc.ElementByteStride = sizeof(uint32);
	bufferDesc.Size = sizeof(uint32) * _countof(vertices);

	indexBuffer = context->CreateBuffer(bufferDesc, commandList);


#endif

	struct Test
	{
		int A = 0;
	};

	Test* a = Memory::Alloc<Test>();
	Memory::FreeDestruct<int>(a);

	int* b = (int*)Memory::Malloc(sizeof(int));
	Memory::Free(b);

	int* c = (int*)Memory::MallocZeroed(sizeof(int));
	Memory::Free(c);


	IAllocator* allocator = new BlockAllocator();

	Test* d = allocator->Alloc<Test>();
	allocator->FreeDestruct<Test>(d);

	delete allocator;
}

void EditorApplication::OnShutdown()
{
	context->~D3D12Context();
	delete context;
	context = nullptr;

	GEngine->Destroy<WindowManager>();
}

void EditorApplication::OnUpdate(float delta)
{
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}