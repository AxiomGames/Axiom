#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include "Axiom/Core/Reflection.hpp"
#include <iostream>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"

#include <entt/entt.hpp>

#include "Axiom/Render/DX12/D3D12Core.hpp"
#include "Axiom/Math/Vector4.hpp"
#include "Axiom/Core/Enum.hpp"

#include <Axiom/Core/Allocator.hpp>
#include <Axiom/Core/Memory.hpp>
#include <Axiom/Core/BlockAllocator.hpp>

AX_ENUM()
enum class TestEnum
{
	One AX_META(DisplayName = "1"),
	Two = 1,
	Three = 1 << 5,
	Four, // This will have value 32 + 1 since 32 is the last one
	Five = 5 AX_META(DisplayName = "Yo")
};

void EditorApplication::OnInit()
{
	/*GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = UINew(UIWindow)
									.AutoCenter(true)
									.Size(Vector2i(1270, 720))
									.Title("Axiom");

	GEngine->Get<WindowManager>()->AddWindow(window, true);

	DX12::Initialize();*/

	/*auto enumClassTest = Reflection::NewType<TestEnum>("TestEnum");

	enumClassTest.Value<TestEnum::One>("One")
		.Attribute(EnumAttributeNumericValue{10});

	enumClassTest.Value<TestEnum::Two>("Two").Attribute(EnumAttributeNumericValue{20}).Attribute(EnumAttributeDisplayName{"asdasd"});*/

	//std::cout << "After " << Reflection::m_TypesByName.Size() << std::endl;

	/*if (auto type = StaticType<::TestEnum>())
	{
		for (auto val : type.Values())
		{
			const EnumAttributeNumericValue& v = val.GetAttribute<EnumAttributeNumericValue>();

			if (val.HasAttribute<EnumAttributeDisplayName>())
			{
				const EnumAttributeDisplayName& displayName = val.GetAttribute<EnumAttributeDisplayName>();

				std::cout << val.GetName().CStr() << " (" << displayName.DisplayName << ")" << " = " << std::to_string(v.NumericValue) << std::endl;
			}
			else
			{
				std::cout << val.GetName().CStr() << " = " << std::to_string(v.NumericValue) << std::endl;
			}
		}
	}*/
}

void EditorApplication::OnShutdown()
{
	/*DX12::Shutdown();

	GEngine->Destroy<WindowManager>();*/
}

void EditorApplication::OnUpdate(float delta)
{
	//DX12::Render();
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}