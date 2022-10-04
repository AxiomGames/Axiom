#include <Axiom/Core/Common.hpp>
#include <Axiom/App/Application.hpp>

class EditorApplication : public Application
{
public:
	void OnInit() override;
	void OnShutdown() override;
};
