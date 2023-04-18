#pragma once

#include "DeviceContext.hpp"
#include <Axiom/App/UI/UIWindow.hpp>

class GraphicsRenderer
{
public:
	GraphicsRenderer(SharedPtr<UIWindow> window);
	~GraphicsRenderer();
	void Render();
private:
	void OnResize(int32 w, int32 h);
	void WaitForGPU();
private:
	ICommandQueue* m_CommandQueue;
	IDeviceContext* m_Context;
	ISwapChain* m_Swapchain;
	ICommandAllocator* m_CommandAllocator;
	ICommandList* m_CommandList;
	SharedPtr<UIWindow> m_Window;
	uint32 m_FrameIndex = 0;
	IPipeline* m_Pipeline;
	uint64 m_FenceValue = 1ull;
	IFence* m_Fence;
	IBuffer* m_IndexBuffer;
	IBuffer* m_VertexBuffer;
    IImage* m_CheckerTexture;
	SharedPtr<INativeWindow> m_NativeWindow;
	PipelineInfo m_PipelineInfo{};
	IBuffer* m_MatrixCBPerFrame[2]{}; // g_NumBackBuffers
};