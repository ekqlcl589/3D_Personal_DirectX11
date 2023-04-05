#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CBone;
class CCollider;

END

BEGIN(Client)
class CProjectileSton final :
	public CGameObject
{
private:
	CProjectileSton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CProjectileSton(const CProjectileSton& rhs);
	virtual ~CProjectileSton() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject * pObj)override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_vector m_vPosition;
	_vector m_vLook;
	_float3 m_fPos;

	_float4x4 m_matWorldMatrix;

	_bool Dead = false;
	_float Damage = 0.f;
	_double m_LifeTime = 0.f;

	//CTransform* pPlayerTransform = { nullptr };
	//
	//_vector vTargetPos;

public:
	static _bool m_bThrow;
	_bool m_RealThrow = false;

public:
	static CProjectileSton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END