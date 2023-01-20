#include "D3D12CommonHeaders.hpp"
#include "d3d12x.h"

class IIndexBuffer { virtual D3D12_INDEX_BUFFER_VIEW* GetViewPtr() = 0; };
class IVertexBuffer { virtual D3D12_VERTEX_BUFFER_VIEW* GetViewPtr() = 0; };

class DX12BufferBase
{
protected:
	ID3D12Resource* mpVidMemBuffer;
	ID3D12Resource* mpSysMemBuffer;
public:
	void Initialize(ID3D12Device2* device, const void* data, uint32 size,
		ID3D12GraphicsCommandList6* pCmd, D3D12_RESOURCE_STATES resourceState,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size, flags);
		DXCall(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			resourceState,
			nullptr, IID_PPV_ARGS(&mpVidMemBuffer)
		));

		auto sysHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		DXCall(device->CreateCommittedResource(
			&sysHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&mpSysMemBuffer)
		));

		// copy to sys
		void* mappedMem;
		D3D12_RANGE range;
		mpSysMemBuffer->Map(0, &range, &mappedMem);
		memcpy(mappedMem, data, size);
		mpSysMemBuffer->Unmap(0, &range);

		// upload to gpu
		auto transion0 = CD3DX12_RESOURCE_BARRIER::Transition(mpVidMemBuffer,
			resourceState,
			D3D12_RESOURCE_STATE_COPY_DEST
		);

		pCmd->ResourceBarrier(1, &transion0);

		pCmd->CopyBufferRegion(mpVidMemBuffer, 0, mpSysMemBuffer, 0, size);

		auto transion1 = CD3DX12_RESOURCE_BARRIER::Transition(mpVidMemBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			resourceState
		);

		pCmd->ResourceBarrier(1, &transion1);
	}
};

template<typename T>
class DX12VertexBuffer : public DX12BufferBase, public IVertexBuffer
{
public:
	D3D12_VERTEX_BUFFER_VIEW view;

	D3D12_VERTEX_BUFFER_VIEW* GetViewPtr() { return &view; }

	DX12VertexBuffer(
		ID3D12Device2* device, const T* data, 
		uint32 size, ID3D12GraphicsCommandList6* pCmd, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		Initialize(device, data, size * sizeof(T), pCmd, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, flags);
		mpVidMemBuffer->SetName(L"AX vertex buffer vid mem");
		mpSysMemBuffer->SetName(L"AX vertex buffer sys mem");

		view.BufferLocation = mpSysMemBuffer->GetGPUVirtualAddress();
		view.SizeInBytes = sizeof(T) * size;
		view.StrideInBytes = sizeof(T);
	}

	~DX12VertexBuffer() {
		ReleaseResource(mpVidMemBuffer);
		ReleaseResource(mpSysMemBuffer);
	}
};

template<typename T>
class DX12IndexBuffer : public DX12BufferBase, public IIndexBuffer
{
public:
	D3D12_INDEX_BUFFER_VIEW view;

	D3D12_INDEX_BUFFER_VIEW* GetViewPtr() { return &view; }

	static inline constexpr DXGI_FORMAT TypeToFormat() {
		// ensure UINT32 or UINT16 indices
		static_assert(sizeof(T) <= 4);
		if constexpr (sizeof(T) == 4) return DXGI_FORMAT_R32_UINT;
		if constexpr (sizeof(T) == 2) return DXGI_FORMAT_R16_UINT;
		return DXGI_FORMAT_R32_UINT;
	}

	DX12IndexBuffer(
		ID3D12Device2* device, const T* data, uint32 size,
		ID3D12GraphicsCommandList6* pCmd, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		Initialize(device, data, size * sizeof(T), pCmd, D3D12_RESOURCE_STATE_INDEX_BUFFER, flags);
		mpVidMemBuffer->SetName(L"HS index buffer vid mem");
		mpSysMemBuffer->SetName(L"HS index buffer sys mem");
		view.BufferLocation = mpSysMemBuffer->GetGPUVirtualAddress();
		view.SizeInBytes = sizeof(T) * size;
		view.Format = TypeToFormat();
	}

	~DX12IndexBuffer() {
		ReleaseResource(mpVidMemBuffer);
		ReleaseResource(mpSysMemBuffer);
	}
};