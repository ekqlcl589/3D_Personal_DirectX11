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
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

public:
	list<class CGameObject*> Get_GameObject() { return m_GameObject; }

private:
	list<class CGameObject*> m_GameObject;
	typedef list<class CGameObject*> GAMEOBJECT;

public:
	static CLayer* Create();
	virtual void Free()override;
};

END