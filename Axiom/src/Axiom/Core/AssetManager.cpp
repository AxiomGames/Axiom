#include "AssetManager.hpp"
#include "String.hpp"

static String ProjectPath;

void AssetManager::Initialize(char* projectPath)
{
	ProjectPath = String(projectPath);
}
