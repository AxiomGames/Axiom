#include "EditorApplication.hpp"

AX_NAMESPACE

void EditorApplication::OnInit()
{

}

AX_END_NAMESPACE

ax::Application* CreateApplication(int argc, char* argv[])
{
	return new ax::EditorApplication();
}