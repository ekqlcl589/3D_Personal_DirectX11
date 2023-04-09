#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CCollider;
class CBone;
class CTransform;

END

BEGIN(Client)

class CAddCollision final:
	public CGameObject
{
public:
	typedef struct tagCollDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;

	}COLLDESC;

private:
	CAddCollision(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAddCollision(const CAddCollision& rhs);
	virtual ~CAddCollision() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject* pObj) override;

private:
	HRESULT Add_Components();

	CTransform*	m_pTransformCom = { nullptr };

public:
	COLLDESC m_Weapon;
	_float4x4 m_WorldMatrix;

public:
	static CAddCollision* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END