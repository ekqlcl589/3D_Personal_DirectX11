#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CTransform;
class CModel;
END

BEGIN(Client)

class CTargetTextrue final :
	public CGameObject
{
private:
	CTargetTextrue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargetTextrue(const CTargetTextrue& rhs);
	virtual ~CTargetTextrue() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:

	_float3 m_fPosition = { 0.f, 0.f, 0.f };

	_double m_LifeTime = 0.0;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* 원형객체를 생성하낟.(추후 사본객체가 복사하기위한 대상으로서)  */
	static CTargetTextrue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성하낟. */
	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END