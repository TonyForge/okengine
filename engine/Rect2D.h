#pragma once

namespace ok
{
	template <class T>
	class Rect2D
	{
	public:
		friend class ok::Rect2D<int>;
		friend class ok::Rect2D<float>;

		Rect2D();
		Rect2D(T _x, T _y, T _w, T _h, bool _yup = false);
		void SetXYWH(T _x, T _y, T _w, T _h);
		void SetLTRB(T _l, T _t, T _r, T _b);
		void GetCenter(T& out_x, T& out_y);
		void ResizeFromCenter(T new_width, T new_height);
		void ResetMerge();
		void Merge(ok::Rect2D<int>& rect);
		void Merge(ok::Rect2D<float>& rect);
		T GetWidth();
		T GetHeight();
		void SetWidth(T width);
		void SetHeight(T height);
		void SetYUp(bool state);
		bool GetYUp();
		void Validate();
		T GetLeft();
		T GetRight();
		T GetTop();
		T GetBottom();
		T GetX();
		T GetY();
		void GetLTRB(T& _l, T& _t, T& _r, T& _b);
		glm::tvec4<T, glm::packed_highp> GetXYWH();
		glm::tvec2<T, glm::packed_highp> GetXY();
		glm::tvec2<T, glm::packed_highp> GetSize();
		glm::tvec2<T, glm::packed_highp> GetCenter();
		glm::tvec2<T, glm::packed_highp> PickRayFromCenter(glm::tvec2<T, glm::packed_highp> ray);
		bool IsOverlap(ok::Rect2D<T>& other);
		ok::Rect2D<T> GetOverlap(ok::Rect2D<T>& other);

		bool Contains(float _x, float _y);
		//operator = with yup difference realisation
	protected:
	private:
		T x, y, w, h;
		bool yup;
		bool merge_empty;
	};

	typedef Rect2D<int> Rect2Di;
	typedef Rect2D<float> Rect2Df;
}