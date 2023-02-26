#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4099)
#pragma warning (disable : 4251)

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <process.h>
#include <d3dcompiler.h>
#include <typeinfo.h>
//#include <typeinfo>

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"

#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

// 모델 로드 테스트용 나중에는 빼야함 
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
//

#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>


using namespace std;
using namespace DirectX;

#include "Engine_Typedef.h"
#include "Engine_Template.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"


#define KEY_MAX 0xff
namespace Engine
{
	enum MOUSEKEYSTATE { DIMK_LB, DIMK_RB, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

}

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


using namespace Engine;