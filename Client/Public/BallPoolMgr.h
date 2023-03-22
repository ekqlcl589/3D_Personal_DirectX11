#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CBallPoolMgr :
	public CBase
{
	DECLARE_SINGLETON(CBallPoolMgr)

private:
	CBallPoolMgr();
	virtual ~CBallPoolMgr() = default;

public:
	// 현재 풀에 있는 오브젝트를 재사용하는 함수
	CGameObject* Reuse_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);

	// 사용을 마친 오브젝트를 회수하는 함수
	void Collect_Object(CGameObject* pObj);


public:
	virtual void Free(void)override;

private:
	list<CGameObject*>	m_ObjectPool;
	_uint	m_iCount = 0;

};

END