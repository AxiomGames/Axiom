#include "AssetManager.hpp"
#include "String.hpp"

AX_NAMESPACE

static String ProjectPath;

void AssetManager::Initialize(char* projectPath)
{
	ProjectPath = String(projectPath);
}

AX_END_NAMESPACE
