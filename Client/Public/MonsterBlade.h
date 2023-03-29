#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
// 이펙트 들의 부모로 나중에 abstract로 바꾸고 사용
class CMonsterBlade final :
	public CGameObject
{
protected:
	CMonsterBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBlade(const CMonsterBlade& rhs);
	virtual ~CMonsterBlade() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject* pObj)override;

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

protected:
	_bool m_bActive = false;
	_int m_Alpha = 100;
	_int m_fFadeSpeed = 2;
	_bool m_bFadeIn = true;
	_bool Dead = false;

	_float3 m_fPosition = { 0.f, 0.f, 0.f };

private:
	_float4x4 m_WorldMatrix;

public:
	static CMonsterBlade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END