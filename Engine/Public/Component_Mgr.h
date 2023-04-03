#pragma once

#include "VIBuffer_Rect.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "Transform.h"
#include "Calulator.h"
#include "Collider.h"
#include "Model.h"

BEGIN(Engine)

class CComponent_Mgr final : public CBase
{
	DECLARE_SINGLETON(CComponent_Mgr)
private:
	CComponent_Mgr();
	virtual ~CComponent_Mgr() = default;

public: //컴포넌트를 사용하는 객체들을 레벨별로 관리, 데이터 공간 예약
	HRESULT Reserve_Mgr(_uint iNumLevel);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Componenet(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

private:
	unordered_map<const _tchar*, class CComponent*>* m_pPrototypes = { nullptr };
	typedef unordered_map<const _tchar*, class CComponent*> PROTOTYPES;

	_uint m_iNumLevels = { 0 };

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
public:
	virtual void Free()override;
};

END