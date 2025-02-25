#include "RectF.h"

DirectX::SimpleMath::Rectangle RectF::toRectangle()
{
	return DirectX::SimpleMath::Rectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h));
}
