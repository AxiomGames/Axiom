#include "Renderer.hpp"
#include "Axiom/Math/Matrix.hpp"
#include <fstream>

#ifdef AX_WIN32
#include "DX12/D3D12Context.hpp"
#endif

// todo: AX::FileSystem, I think we should use c standart library for this because it has smaller code generation and faster compile times
inline char* ReadAllFile(StringView fileName)
{
	FILE* fp = fopen(fileName.data(), "r");
	if (!fp) {
		printf("file read failed");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	rewind(fp);
	// BOM check, Byte order mark
	{
		unsigned char test[3] = { 0 };
		fread(test, 1, 3, fp);

		if (test[0] != 0xEF || test[1] != 0xBB || test[2] != 0xBF) {
			rewind(fp);
		}
	}

	char* fcontent = (char*)calloc((size_t)(fileSize + 1l), 1ull);
	fread(fcontent, 1, fileSize, fp);
	fclose(fp);
	return fcontent;
}

GraphicsRenderer::GraphicsRenderer(SharedPtr<UIWindow> window)
{
	struct VertexPosColor { Vector3f position, color; };

	const VertexPosColor vertices[8] = {
		{ Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(0.0f, 0.0f, 0.0f) }, // 0
		{ Vector3f(-1.0f,  1.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f) }, // 1
		{ Vector3f( 1.0f,  1.0f, -1.0f), Vector3f(1.0f, 1.0f, 0.0f) }, // 2
		{ Vector3f( 1.0f, -1.0f, -1.0f), Vector3f(1.0f, 0.0f, 0.0f) }, // 3
		{ Vector3f(-1.0f, -1.0f,  1.0f), Vector3f(0.0f, 0.0f, 1.0f) }, // 4
		{ Vector3f(-1.0f,  1.0f,  1.0f), Vector3f(0.0f, 1.0f, 1.0f) }, // 5
		{ Vector3f( 1.0f,  1.0f,  1.0f), Vector3f(1.0f, 1.0f, 1.0f) }, // 6
		{ Vector3f( 1.0f, -1.0f,  1.0f), Vector3f(1.0f, 0.0f, 1.0f) }  // 7
	};

	const uint32 indices[36] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

#ifdef AX_WIN32
	m_Context = new D3D12Context();
#endif
	m_Window = window;
	m_NativeWindow = window->GetNativeWindow();
	m_Context->Initialize(m_NativeWindow);
	// todo: nativeWindow->AddOnResize(OnResize);

	m_CommandQueue = m_Context->CreateCommandQueue(ECommandListType::Direct, ECommandQueuePriority::Normal);
	m_Swapchain = m_Context->CreateSwapChain(m_CommandQueue, EImageFormat::RGBA8);
	// we may want to store command allocator and fence value in struct called frame context
	m_CommandAllocator = m_Context->CreateCommandAllocator(ECommandListType::Direct);
	// todo create for each thread
	m_CommandList = m_Context->CreateCommandList(m_CommandAllocator, ECommandListType::Direct);

	BufferDesc bufferDesc{};
	bufferDesc.Data = (void*)vertices;
	bufferDesc.ResourceUsage = EResourceUsage::VertexBuffer;
	bufferDesc.ElementByteStride = sizeof(VertexPosColor);
	bufferDesc.Size = sizeof(VertexPosColor) * _countof(vertices);
	m_VertexBuffer = m_Context->CreateBuffer(bufferDesc, m_CommandList);

	bufferDesc.Data = (void*)indices;
	bufferDesc.ResourceUsage = EResourceUsage::IndexBuffer;
	bufferDesc.ElementByteStride = sizeof(uint32);
	bufferDesc.Size = sizeof(uint32) * _countof(indices);
	m_IndexBuffer = m_Context->CreateBuffer(bufferDesc, m_CommandList);

	char* shaderCode = ReadAllFile("../EngineAssets/Shaders/PBR.hlsl");
	m_PipelineInfo.VertexShader = m_Context->CreateShader(shaderCode, "VS", EShaderType::Vertex);
	m_PipelineInfo.FragmentShader = m_Context->CreateShader(shaderCode, "PS", EShaderType::Fragment);
	m_PipelineInfo.numRenderTargets = 1;
	m_PipelineInfo.RTVFormats[0] = EImageFormat::RGBA8;
    m_PipelineInfo.DepthStencilFormat = EImageFormat::D32F;

	m_PipelineInfo.numInputLayout = 2;
	m_PipelineInfo.inputLayouts[0] = { "POSITION", VertexAttribType::Float3 };
	m_PipelineInfo.inputLayouts[1] = { "COLOR"   , VertexAttribType::Float3 };

	m_Pipeline = m_Context->CreateGraphicsPipeline(m_PipelineInfo);
	m_Fence = m_Context->CreateFence();

	m_CommandList->Close();
	m_CommandQueue->ExecuteCommandLists(&m_CommandList, 1);

	WaitForGPU();
	free(shaderCode);
}

void GraphicsRenderer::Render()
{
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, m_Pipeline);

	ViewportDesc viewport(0.0f, 0.0f, (float)m_NativeWindow->GetWidth(), (float)m_NativeWindow->GetHeight());
	GraphicsRect rect(0u, 0u, (uint32)m_NativeWindow->GetWidth(), (uint32)m_NativeWindow->GetHeight(), 0u, 0u);
	m_CommandList->SetPipelineState(m_Pipeline);
	m_CommandList->SetViewports(1, &viewport);
	m_CommandList->SetScissorRects(1, &rect);

	PipelineBarrier barrier{};
	barrier.CurrentStage = EPipelineStage::Present;
	barrier.NextStage = EPipelineStage::RenderTarget;
	IImage* backBuffer = m_Swapchain->GetBackBuffer(m_FrameIndex);
	IImage* depthStencilBuffer = m_Swapchain->GetDepthStencilBuffer();
	m_CommandList->SetImageBarrier(backBuffer, barrier);
	m_CommandList->SetRenderTargets(&backBuffer, 1, depthStencilBuffer);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_CommandList->ClearRenderTarget(backBuffer, clearColor);
	m_CommandList->ClearDepthStencil(depthStencilBuffer);

	// rotate camera around our cube
	static float f = 1.0f;
	f += 0.01f;
	Vector3f position(sinf(f) * 5.0f, 0.0f, cosf(f) * 5.0f);
	float verticalFOV = 65.0f;
	float nearClip = 0.01f;
	float farClip = 500.0f;

	Matrix4 projection = Matrix4::PerspectiveFovRH(verticalFOV * Math::DegToRad, m_NativeWindow->GetWidth(), m_NativeWindow->GetHeight(), nearClip, farClip);
	Matrix4 viewProjection = projection * Matrix4::LookAtRH(position, -Vector3f::Normalize(position), Vector3f::Up());
	
	// todo seperate this task in between threads
	m_CommandList->SetIndexBuffer(m_IndexBuffer);
	m_CommandList->SetVertexBuffers(&m_VertexBuffer, 1);
	m_CommandList->SetGraphicsPushConstants(m_Pipeline, EShaderType::Vertex, &viewProjection, sizeof(Matrix4));
	
	m_CommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	barrier.CurrentStage = EPipelineStage::RenderTarget;
	barrier.NextStage = EPipelineStage::Present;
	m_CommandList->SetImageBarrier(backBuffer, barrier);
	m_CommandList->Close();

	m_CommandQueue->ExecuteCommandLists(&m_CommandList, 1);

	m_Swapchain->Present(true, 0u);

	WaitForGPU();
	m_FrameIndex ^= 1; // double buffering repeats 0,1,0,1	
}

void GraphicsRenderer::WaitForGPU()
{
	m_CommandQueue->Signal(m_Fence, m_FenceValue);
	m_Context->WaitFence(m_Fence, m_FenceValue++);
}

void GraphicsRenderer::OnResize(int w, int h)
{
	// todo:
}

GraphicsRenderer::~GraphicsRenderer()
{
	m_Context->DestroyResource(m_IndexBuffer);
	m_Context->DestroyResource(m_VertexBuffer);
	m_Context->DestroyResource(m_PipelineInfo.VertexShader);
	m_Context->DestroyResource(m_PipelineInfo.FragmentShader);
	m_Context->DestroyResource(m_Swapchain);
	m_Context->DestroyResource(m_CommandQueue);
	m_Context->DestroyResource(m_CommandList);
	m_Context->DestroyResource(m_CommandAllocator);
	m_Context->Release();

	delete m_Context;
}