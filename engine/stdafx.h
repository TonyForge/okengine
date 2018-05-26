#pragma once

#include <limits>
#include <list>
#include <bitset>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <queue>
#include <stack>
#include <memory>
#include <algorithm>
#include <tuple>
#include <typeindex>
#include <typeinfo>

#include <Windows.h>

#include "SFML\System.hpp"
#include "SFML\Window.hpp"
#include "GL\glew.h"
#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include "SFML\OpenGL.hpp"

#include "tinyxml\tinyxml2.h"

#include "easylogging++.h"

//#define GLM_FORCE_SWIZZLE
//#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "glm\gtx\norm.hpp"
#include "glm\gtx\euler_angles.hpp"
#include "glm\gtx\vector_angle.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"

#include "String.h"