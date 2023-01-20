#pragma once

#include "D3D12CommonHeaders.hpp"
#include "DX12Shader.hpp"
#include "DX12Buffers.hpp"
#include "Axiom/Math/Vector4.hpp"

class DX12Renderer
{
public:
	DX12Renderer(ID3D12Device8* _device, ID3D12DescriptorHeap* _srvHeap, ID3D12GraphicsCommandList6* commandList);
	void Render(const CD3DX12_VIEWPORT& vp);
private:
	struct PBRVertex {
		Vector3f position;
		Vector4 color;
	};

	ID3D12Device8* m_Device;
	ID3D12RootSignature* m_RootSignature;
	ID3D12PipelineState* m_PipelineState;

	ID3D12DescriptorHeap* m_SrvHeap;
	ID3D12GraphicsCommandList6* m_CommandList;

	DX12Shader* m_PBRShader;
	DX12VertexBuffer<PBRVertex>* m_VertexBuffer;
	DX12IndexBuffer<uint32>* m_IndexBuffer;
};