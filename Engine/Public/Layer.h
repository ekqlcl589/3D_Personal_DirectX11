#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)
class CLayer final :
	public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
	HRESULT Delete_GameObject(CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

public:
	list<class CGameObject*> Get_GameObject() { return m_GameObject; }

public:
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);

private:
	list<class CGameObject*> m_GameObject;
	typedef list<class CGameObject*> GAMEOBJECT;

	_bool a = false;
public:
	static CLayer* Create();
	virtual void Free()override;
};

END