
#include "D3D12Renderer.hpp"

DX12Renderer::DX12Renderer(ID3D12Device8* _device, ID3D12DescriptorHeap* _srvHeap, ID3D12GraphicsCommandList6* commandList)
	: m_Device(_device), m_CommandList(commandList), m_SrvHeap(_srvHeap)
{
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

	m_VertexBuffer = new DX12VertexBuffer<PBRVertex>(_device, vertices, _countof(vertices), commandList);
	m_IndexBuffer = new DX12IndexBuffer<uint32>(_device, indices, _countof(indices), commandList);

	m_PBRShader = new DX12Shader("../EngineAssets/Shaders/PBR.hlsl");

	HRESULT hr;
	// Create the root signature
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		const D3D12_ROOT_SIGNATURE_FLAGS flags
			= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		rootSignatureDesc.Init(0, nullptr, 0, nullptr, flags);

		ID3DBlob* signature = nullptr;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		if (FAILED(hr)) throw std::exception("root signature creation failed!");
		hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		if (FAILED(hr)) throw std::exception("root signature creation failed!");
	}

	const uint32 msaaQuality = 0; // D3D12Backend::GetMSAAQuality();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	memset(&psoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.NodeMask = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Quality = msaaQuality ? (msaaQuality - 1) : 0;
	psoDesc.SampleDesc.Count = msaaQuality ? msaaQuality : 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	psoDesc.VS = { m_PBRShader->vertexShaderBlob->GetBufferPointer(), m_PBRShader->vertexShaderBlob->GetBufferSize() };

	static D3D12_INPUT_ELEMENT_DESC local_layout[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, (UINT)offsetof(PBRVertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(PBRVertex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	psoDesc.InputLayout = { local_layout, _countof(local_layout) };

	psoDesc.PS = { m_PBRShader->fragmentShaderBlob->GetBufferPointer(), m_PBRShader->fragmentShaderBlob->GetBufferSize() };

	// Create the blending setup
	{
		D3D12_BLEND_DESC& desc = psoDesc.BlendState;
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	// Create the rasterizer state
	
	{
		D3D12_RASTERIZER_DESC& desc = psoDesc.RasterizerState;
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_NONE;
		desc.FrontCounterClockwise = FALSE;
		// desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		// desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		// desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		// desc.DepthClipEnable = true;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount = 0;
		desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	// Create depth-stencil State
	if constexpr (false)
	{
		D3D12_DEPTH_STENCIL_DESC& desc = psoDesc.DepthStencilState;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.BackFace = desc.FrontFace;
	}

	DXCall(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	m_PBRShader->ReleaseBlobs();
}

void DX12Renderer::Render(const CD3DX12_VIEWPORT& vp)
{
	// setup render state
	D3D12_RECT rect = { 0, 0, vp.Width, vp.Height };

	m_CommandList->RSSetViewports(1, &vp);	
	m_CommandList->RSSetScissorRects(1, &rect);
	m_CommandList->IASetVertexBuffers(0, 1, m_VertexBuffer->GetViewPtr());
	m_CommandList->IASetIndexBuffer(m_IndexBuffer->GetViewPtr());
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetPipelineState(m_PipelineState);
	m_CommandList->SetGraphicsRootSignature(m_RootSignature);

	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_CommandList->OMSetBlendFactor(blend_factor);

	m_CommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
