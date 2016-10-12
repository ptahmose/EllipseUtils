#pragma once

namespace EllipseUtils
{
	template<typename tFloat>
	struct Rect
	{
		tFloat x;
		tFloat y;
		tFloat w;
		tFloat h;
	};

	typedef Rect<float> RectF;
	typedef Rect<double> RectD;
}
