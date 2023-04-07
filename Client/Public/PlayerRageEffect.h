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

class CPlayerRageEffect final :
	public CGameObject
{
private:
	CPlayerRageEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerRageEffect(const CPlayerRageEffect& rhs);
	virtual ~CPlayerRageEffect() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_float4x4 m_WorldMatrix;
	_bool m_bActive = false;

public:
	static CPlayerRageEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END