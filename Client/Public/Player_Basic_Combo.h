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
class CPlayer_Basic_Combo final :
	public CGameObject
{
protected:
	CPlayer_Basic_Combo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Basic_Combo(const CPlayer_Basic_Combo& rhs);
	virtual ~CPlayer_Basic_Combo() = default;

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

protected:
	_bool m_bActive = false;
	_int m_Alpha = 100;
	_int m_fFadeSpeed = 2;
	_bool m_bFadeIn = true;
	_bool m_bDead = false;

private:
	_float4x4 m_WorldMatrix;

	_bool m_AnimStart = false;

public:
	static CPlayer_Basic_Combo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END