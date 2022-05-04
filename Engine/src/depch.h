#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Engine/Core/Application.h"

#include "Engine/Core/Timestep.h"
#include "Engine/Core/Log.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Core/Input.h"
#include "Engine/Renderer/Texture.h"

#ifdef ENGINE_PLATFORM_WINDOWS
	#include <Windows.h>
#endif