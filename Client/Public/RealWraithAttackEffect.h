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

class CRealWraithAttackEffect final :
	public CGameObject
{
private:
	CRealWraithAttackEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CRealWraithAttackEffect(const CRealWraithAttackEffect& rhs);
	virtual ~CRealWraithAttackEffect() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_vector m_vPosition;
	_bool m_bActive = false;

	_double m_LifeTime = 0.0;

public:
	static CRealWraithAttackEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END