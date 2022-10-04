#pragma once

#include "Axiom/Core/Optional.hpp"
#include "Axiom/Math/Vector2.hpp"

#define BEGIN_WIDGET_ARGS(WidgetType) \
	public: \
		struct WArguments \
		{ \
			WArguments()

#define END_WIDGET_ARGS \
	};

#define WIDGET_ATTRIBUTE(Type, Name) \
	Type m_##Name;


using LayerID = uint32;

class Widget : public SharedFromThis<Widget>
{
private:
	WeakPtr<Vector2i> m_ParentWidgetPtr;

	Vector2i m_Location;
	Optional<Vector2i> m_DesiredSize;
public:
	LayerID Paint() const;
	virtual void Tick(float currentTime, float deltaTime);
private:
	virtual LayerID OnPaint() const = 0;
};
