#include "Renderer.hpp"
#include "Axiom/Math/Matrix.hpp"
#include "Axiom/Core/Random.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

struct Vertex {
    Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}
	Vector3f pos;
	Vector2f texCoord;
};

GraphicsRenderer::GraphicsRenderer(SharedPtr<UIWindow> window)
{
    static const Vertex vertices[] = 
    {
        { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 1.0f, 0.0f },
        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },
        {  0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
        {  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
        {  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
    };

    static const uint32 indices[] = {
        0, 1, 2,    
        0, 3, 1,    
        4, 5, 6,    
        4, 7, 5,    
        8, 9, 10,   
        8, 11, 9,   
        12, 13, 14, 
        12, 15, 13, 
        16, 17, 18, 
        16, 19, 17, 
        20, 21, 22, 
        20, 23, 21, 
    };

#ifdef AX_WIN32
	m_Context = new D3D12Context();
#endif
    m_Window = window;
    m_NativeWindow = window->GetNativeWindow();
    m_Context->Initialize(m_NativeWindow);
    // todo: nativeWindow->AddOnResize(OnResize);
    
    m_CommandQueue = m_Context->CreateCommandQueue(ECommandListType::Direct, ECommandQueuePriority::Normal);
    m_Swapchain = m_Context->CreateSwapChain(m_CommandQueue, EGraphicsFormat::RGBA8);
    // we may want to store command allocator and fence value in struct called frame context
    // iedoc has seperate allocators for each frame
    m_CommandAllocator = m_Context->CreateCommandAllocator(ECommandListType::Direct);
    // todo create for each thread
    m_CommandList = m_Context->CreateCommandList(m_CommandAllocator, ECommandListType::Direct);
    
    int width, height, comp;
    stbi_uc* image = stbi_load("../EngineAssets/Images/earthmap.jpg", &width, &height, &comp, 4);
    
    BufferDesc imageDesc{};
    imageDesc.Data = image;
    imageDesc.Format = EGraphicsFormat::RGBA8;
    imageDesc.ResourceUsage = EResourceUsage::ShaderResource;
    imageDesc.BufferType = EBufferType::Texture2D;
    imageDesc.Width = width;
    imageDesc.Height = height;
    m_CheckerTexture = m_Context->CreateImage(imageDesc, m_CommandList);
    
    // create mvp constant buffers for each frame
    for (int i = 0; i < g_NumBackBuffers; ++i)
    {
    	BufferDesc cbDesc{};
    	cbDesc.BufferType = EBufferType::Buffer;
    	cbDesc.Width = sizeof(Matrix4);
    	cbDesc.FrameIndex = i;
    	cbDesc.ResourceUsage = EResourceUsage::ConstantBuffer;
    	cbDesc.flags = EBufferDescFlags::DirectUpload;
        m_MatrixCBPerFrame[i] = m_Context->CreateBuffer(cbDesc, m_CommandList);
    }
    
    BufferDesc bufferDesc{};
    bufferDesc.Data = (void*)vertices;
    bufferDesc.ResourceUsage = EResourceUsage::VertexBuffer;
    bufferDesc.ElementByteStride = sizeof(Vertex);
    bufferDesc.Width = sizeof(Vertex) * _countof(vertices);
    bufferDesc.Height = 1;
    bufferDesc.BufferType = EBufferType::Buffer;
    bufferDesc.Format = EGraphicsFormat::UNKNOWN;
    m_VertexBuffer = m_Context->CreateBuffer(bufferDesc, m_CommandList);
    
    bufferDesc.Data = (void*)indices;
    bufferDesc.ResourceUsage = EResourceUsage::IndexBuffer;
    bufferDesc.ElementByteStride = sizeof(uint32);
    bufferDesc.Width = sizeof(uint32) * _countof(indices);
    bufferDesc.Format = EGraphicsFormat::UNKNOWN;
    m_IndexBuffer = m_Context->CreateBuffer(bufferDesc, m_CommandList);
    
    char* shaderCode = ReadAllFile("../EngineAssets/Shaders/PBR.hlsl");
    m_PipelineInfo.VertexShader = m_Context->CreateShader(shaderCode, "VS", EShaderType::Vertex);
    m_PipelineInfo.FragmentShader = m_Context->CreateShader(shaderCode, "PS", EShaderType::Pixel);
    m_PipelineInfo.numRenderTargets = 1;
    m_PipelineInfo.RTVFormats[0] = EGraphicsFormat::RGBA8;
    m_PipelineInfo.DepthStencilFormat = EGraphicsFormat::D32F;
    
    m_PipelineInfo.numInputLayout = 2;
    m_PipelineInfo.inputLayouts[0] = { "POSITION", VertexAttribType::Float3 };
    m_PipelineInfo.inputLayouts[1] = { "TEXCOORD", VertexAttribType::Float2 };
    
    DescriptorSetDesc& descriptorDesc = m_PipelineInfo.descriptorSet;
    descriptorDesc.BindingCount = 2;
    descriptorDesc.Bindings[0].Type = EDescriptorType::ConstantBuffer;
    descriptorDesc.Bindings[0].ShaderVisibility = EShaderType::Vertex;
    descriptorDesc.Bindings[1].Type = EDescriptorType::ShaderResource;
    descriptorDesc.Bindings[1].ShaderVisibility = EShaderType::Pixel;
    descriptorDesc.NumSamplers = 1;
    descriptorDesc.Samplers[0].BorderColor = ETextureBorderColor::OpaqueBlack;
    descriptorDesc.Samplers[0].ShaderVisibility = EShaderType::Pixel;
    descriptorDesc.Samplers[0].AdressU = descriptorDesc.Samplers[0].AdressV = 
                                         descriptorDesc.Samplers[0].AdressW = ETextureAddressMode::ClampToEdge;
    // descriptorDesc.Samplers[0].MagFilter = EFiltering::Nearest; // nearest for default
    
    m_Pipeline = m_Context->CreateGraphicsPipeline(m_PipelineInfo);
    // some people has seperate fences for each frame
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
	m_CommandList->SetViewports(1,	 &viewport);
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
	static float f = 1.0f; f += 0.01f;
	constexpr float distance = 3.14159265f; // this is distance from cube but I did use pi anyways 
	Vector3f position(sinf(f) * distance, 0.0f, cosf(f) * distance );
	float verticalFOV = 65.0f, nearClip = 0.01f, farClip = 500.0f;

	Matrix4 projection = Matrix4::PerspectiveFovRH(verticalFOV * Math::DegToRad, m_NativeWindow->GetWidth(), m_NativeWindow->GetHeight(), nearClip, farClip);
	Matrix4 viewProjection = projection * Matrix4::LookAtRH(position, -Vector3f::Normalize(position), Vector3f::Up());
	
	// todo seperate this task in between threads
	m_CommandList->SetIndexBuffer(m_IndexBuffer);
	m_CommandList->SetVertexBuffers(&m_VertexBuffer, 1);
    m_Context->MapBuffer(m_MatrixCBPerFrame[m_FrameIndex], &viewProjection, sizeof(Matrix4));
    
	m_CommandList->SetConstantBufferView(0, m_MatrixCBPerFrame[m_FrameIndex]); // 0th thing in our descriptor set is cbuffer
    m_CommandList->SetTexture(1, m_CheckerTexture); // 1th thing in our descriptor set is texture

    // m_CommandList->SetGraphicsPushConstants(m_Pipeline, EShaderType::Vertex, &viewProjection, sizeof(Matrix4));
	
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