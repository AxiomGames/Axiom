#pragma once

#include "D3D12CommonHeaders.hpp"

class DX12Shader
{
public:
	ID3DBlob* vertexShaderBlob, *fragmentShaderBlob;
public:
	DX12Shader(const char* path, const char* VS_Name = "VS", const char* PS_NAME = "PS")
	{
		Initialize(path, VS_Name, PS_NAME);
	}
public:
	void ReleaseBlobs() {
		ReleaseResource(vertexShaderBlob);
		ReleaseResource(fragmentShaderBlob);
	}
private:
	void Initialize(const char* path, const char* VS_Name = "VS", const char* PS_NAME = "PS");
};