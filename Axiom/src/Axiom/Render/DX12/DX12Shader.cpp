#include "DX12Shader.hpp"
#include <exception>
#include <fstream>

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

void DX12Shader::Initialize(const char* path, const char* VS_Name, const char* PS_NAME)
{
	const char* vertexShader = ReadAllFile(path);
	const char* fragmentShader = vertexShader; // if we want we can load from different file

	ID3DBlob* vertexErrorBlob, *fragErrorBlob;
#ifdef  DEBUG
		D3D_SHADER_MACRO shaderMacros[] = { {"DEBUG", "1"}, {nullptr, nullptr}};
#else 
		D3D_SHADER_MACRO shaderMacros[] = { {"RELEASE", "1"}, {nullptr, nullptr} };
#endif //  DEBUG

	if (FAILED(D3DCompile(vertexShader, strlen(vertexShader), nullptr,
		shaderMacros, nullptr, VS_Name, "vs_5_0", 0, 0, &vertexShaderBlob, &vertexErrorBlob)))
	{
        AX_ERROR("Vertex Shader Compiling Error:\n %s", (char*)vertexErrorBlob->GetBufferPointer());
		throw std::exception("vertex shader compiling failed!");
	}

	if (FAILED(D3DCompile(fragmentShader, strlen(fragmentShader), nullptr,
		shaderMacros, nullptr, PS_NAME, "ps_5_0", 0, 0, &fragmentShaderBlob, &fragErrorBlob)))
	{
		AX_ERROR("Pixel Shader Compiling Error:\n %s", (char*)fragErrorBlob->GetBufferPointer());
		throw std::exception("Pixel shader compiling failed!");
	}
}