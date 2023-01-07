#pragma once

#include "Axiom/CoreMinimal.hpp"

// Widget args helpers
template<typename T>
class UIAttribute : public Optional<T>
{

};

#define BEGIN_WIDGET_ARGS(WidgetType) \
	public: \
		struct WArguments \
		{ \
			WArguments()

#define END_WIDGET_ARGS \
	};

#define WIDGET_ATTRIBUTE(Type, Name) \
	Type m_##Name;            \
	WArguments& Name(const Type& val)          \
	{ m_##Name = val; return *this; }

// Widget construction
template<typename WidgetType>
class WidgetConstructor
{
private:
	SharedPtr<WidgetType> m_Widget;
public:

	WidgetConstructor() : m_Widget(new WidgetType())
	{

	}

	WidgetConstructor& ExposeAs(SharedPtr<WidgetType>& asposeAs)
	{
		asposeAs = m_Widget;
	}

	SharedPtr<WidgetType> operator<<=(const typename WidgetType::WArguments& args)
	{
		m_Widget->Construct(args);
		return m_Widget;
	}
};

#define UINew(Type) WidgetConstructor<Type>() <<= typename Type::WArguments()
#define UINewAssign(Var, Type) WidgetConstructor<Type>().ExposeAs(Var) <<= typename Type::WArguments();

// Begin widget
using LayerID = uint32;

class Widget : public SharedFromThis<Widget>
{
protected:
	WeakPtr<Vector2i> m_ParentWidgetPtr;

	Vector2i m_Location;
	Optional<Vector2i> m_DesiredSize;
public:
	LayerID Paint() const;
	virtual void Tick(float currentTime, float deltaTime);
private:
	virtual LayerID OnPaint() const = 0;
};
