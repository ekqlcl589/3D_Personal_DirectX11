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
	// ���� Ǯ�� �ִ� ������Ʈ�� �����ϴ� �Լ�
	CGameObject* Reuse_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);

	// ����� ��ģ ������Ʈ�� ȸ���ϴ� �Լ�
	void Collect_Object(CGameObject* pObj);


public:
	virtual void Free(void)override;

private:
	list<CGameObject*>	m_ObjectPool;
	_uint	m_iCount = 0;

};

END