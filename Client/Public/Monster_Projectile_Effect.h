#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)
// 이펙트 들의 부모로 나중에 abstract로 바꾸고 사용
class Monster_Projectile_Effect final :
	public CGameObject
{
protected:
	Monster_Projectile_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Monster_Projectile_Effect(const Monster_Projectile_Effect& rhs);
	virtual ~Monster_Projectile_Effect() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	_bool FadeInOut();

public:
	void Set_Transform();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_bool m_bActive = false;
	_int m_Alpha = 100;
	_int m_fFadeSpeed = 2;
	_bool m_bFadeIn = true;

private:
	_float4x4 m_WorldMatrix;

	_vector m_vPosition;
	_float3 m_fPosition = { 0.f, 0.f, 0.f };
public:
	static Monster_Projectile_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END