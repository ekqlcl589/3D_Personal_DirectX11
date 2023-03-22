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

class CBlackBall final :
	public CGameObject
{
private:
	CBlackBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBlackBall(const CBlackBall& rhs);
	virtual ~CBlackBall() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bStart = { false };

private:
	_vector m_vPosition;
	_vector m_vLook;

	_bool Dead = false;
	_float Damage = 0.f;
	_double m_LifeTime = 5.f;

public:
	static CBlackBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END