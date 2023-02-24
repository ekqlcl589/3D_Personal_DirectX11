#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObject_Manager final :	public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);

	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);

	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);

	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

	void Clear(_uint iLevelIndex);

public:
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);

	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);

	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);

private: //¿øº»°´Ã¼
	unordered_map<const _tchar*, class CGameObject*> m_Prototypes;

private: //»çº»°´Ã¼
	unordered_map<const _tchar*, class CLayer*>* m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*> LAYERS;

private:
	_uint m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex,const _tchar* pLayerTag);

public:
	virtual void Free()override;
};

END