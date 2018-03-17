#pragma once

namespace ok
{
	class Spline
	{
	public:
		static ok::Spline& instance();
		static ok::Spline& o();

		static glm::vec3 HermitPick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick);
		static glm::vec3 HermitTangentPick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick);
		static glm::vec3 Hermit1stDerivativePick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick);
		static glm::vec3 Hermit2ndDerivativePick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick);
	protected:
	private:
		Spline() {}
		~Spline() {}
		ok::Spline(ok::Spline const&) {}
		ok::Spline& operator= (ok::Spline const&) {}
	};
}