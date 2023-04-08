#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)
// 이펙트 들의 부모로 나중에 abstract로 바꾸고 사용
class CPlayer_Rage_Arma final :
	public CGameObject
{
protected:
	CPlayer_Rage_Arma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Rage_Arma(const CPlayer_Rage_Arma& rhs);
	virtual ~CPlayer_Rage_Arma() = default;

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
	CTexture* m_pTextureCom = { nullptr };

protected:
	_bool m_bActive = false;
	_bool m_bRage = false;
	_float m_Alpha = 100.f;
	_int m_fFadeSpeed = 2;
	_bool m_bFadeIn = true;
	_bool m_bDead = false;

private:
	_float4x4 m_WorldMatrix;

public:
	static CPlayer_Rage_Arma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END