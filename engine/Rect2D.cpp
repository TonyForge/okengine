#include "Rect2D.h"


template <class T>
void ok::Rect2D<T>::SetYUp(bool state)
{
	yup = state;
}

template <class T>
bool ok::Rect2D<T>::GetYUp()
{
	return yup;
}

template <class T>
void ok::Rect2D<T>::Validate()
{
	if (w < 0)
	{
		w *= -1;
		x -= w;
	}

	if (h < 0)
	{
		h *= -1;
		y -= h;
	}
}

template <class T>
T ok::Rect2D<T>::GetLeft()
{
	return x;
}

template <class T>
T ok::Rect2D<T>::GetRight()
{
	return x + w;
}

template <class T>
T ok::Rect2D<T>::GetTop()
{
	if (yup)
	{
		return y + h;
	}
	return y;
}

template <class T>
T ok::Rect2D<T>::GetBottom()
{
	if (yup)
	{
		return y;
	}
	return y + h;
}

template<class T>
T ok::Rect2D<T>::GetX()
{
	return x;
}

template<class T>
T ok::Rect2D<T>::GetY()
{
	return y;
}

template <class T>
void ok::Rect2D<T>::GetLTRB(T & _l, T & _t, T & _r, T & _b)
{
	_l = x;
	_r = x + w;
	if (yup)
	{
		_t = y + h;
		_b = y;
	}
	else
	{
		_t = y;
		_b = y + h;
	}
}

template<class T>
glm::tvec2<T, glm::packed_highp> ok::Rect2D<T>::GetXY()
{
	return glm::tvec2<T, glm::packed_highp>(x,y);
}

template<class T>
glm::tvec2<T, glm::packed_highp> ok::Rect2D<T>::GetSize()
{
	return glm::tvec2<T, glm::packed_highp>(w, h);
}

template <class T>
ok::Rect2D<T>::Rect2D() : yup(false), merge_empty(false)
{
	x = 0;
	y = 0;
	w = 1;
	h = 1;
}

template <class T>
ok::Rect2D<T>::Rect2D(T _x, T _y, T _w, T _h, bool _yup) : yup(_yup), merge_empty(false)
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

template <class T>
void ok::Rect2D<T>::SetXYWH(T _x, T _y, T _w, T _h)
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

template <class T>
void ok::Rect2D<T>::SetLTRB(T _l, T _t, T _r, T _b)
{
	x = _l;
	w = _r - _l;

	if (yup)
	{
		y = _b;
		h = _t - _b;
	}
	else
	{
		y = _t;
		h = _b - _t;
	}
}

template <class T>
void ok::Rect2D<T>::GetCenter(T & out_x, T & out_y)
{
	out_x = x + w / static_cast<T>(2);
	out_y = y + h / static_cast<T>(2);
}

template <class T>
void ok::Rect2D<T>::ResizeFromCenter(T new_width, T new_height)
{
	GetCenter(x, y);
	x -= new_width / static_cast<T>(2);
	y -= new_height / static_cast<T>(2);
	w = new_width;
	h = new_height;
}

template<class T>
void ok::Rect2D<T>::ResetMerge()
{
	merge_empty = true;
}

template <class T>
void ok::Rect2D<T>::Merge(ok::Rect2D<int>& rect)
{
	if (merge_empty)
	{
		x = static_cast<T>(rect.x);
		y = static_cast<T>(rect.y);
		w = static_cast<T>(rect.w);
		h = static_cast<T>(rect.h);

		merge_empty = false;
	}
	else
	{
		if (yup)
		{
			SetLTRB(
				glm::min(GetLeft(), static_cast<T>(rect.GetLeft())),
				glm::max(GetTop(), static_cast<T>(rect.GetTop())),
				glm::max(GetRight(), static_cast<T>(rect.GetRight())),
				glm::min(GetBottom(), static_cast<T>(rect.GetBottom()))
			);
		}
		else
		{
			SetLTRB(
				glm::min(GetLeft(), static_cast<T>(rect.GetLeft())),
				glm::min(GetTop(), static_cast<T>(rect.GetTop())),
				glm::max(GetRight(), static_cast<T>(rect.GetRight())),
				glm::max(GetBottom(), static_cast<T>(rect.GetBottom()))
			);
		}
	}
}

template <class T>
void ok::Rect2D<T>::Merge(ok::Rect2D<float>& rect)
{
	if (merge_empty)
	{
		x = static_cast<T>(rect.x);
		y = static_cast<T>(rect.y);
		w = static_cast<T>(rect.w);
		h = static_cast<T>(rect.h);

		merge_empty = false;
	}
	else
	{
		if (yup)
		{
			SetLTRB(
				glm::min(GetLeft(), static_cast<T>(rect.GetLeft())),
				glm::max(GetTop(), static_cast<T>(rect.GetTop())),
				glm::max(GetRight(), static_cast<T>(rect.GetRight())),
				glm::min(GetBottom(), static_cast<T>(rect.GetBottom()))
			);
		}
		else
		{
			SetLTRB(
				glm::min(GetLeft(), static_cast<T>(rect.GetLeft())),
				glm::min(GetTop(), static_cast<T>(rect.GetTop())),
				glm::max(GetRight(), static_cast<T>(rect.GetRight())),
				glm::max(GetBottom(), static_cast<T>(rect.GetBottom()))
			);
		}
	}
}

template <class T>
T ok::Rect2D<T>::GetWidth()
{
	return w;
}

template <class T>
T ok::Rect2D<T>::GetHeight()
{
	return h;
}

template <class T>
void ok::Rect2D<T>::SetWidth(T width)
{
	w = width;
}

template <class T>
void ok::Rect2D<T>::SetHeight(T height)
{
	h = height;
}

template class ok::Rect2D<int>;
template class ok::Rect2D<float>;