#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class PlayerRageEffectTexture final :
	public CBlendObject
{
private:
	PlayerRageEffectTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	PlayerRageEffectTexture(const PlayerRageEffectTexture& rhs);
	virtual ~PlayerRageEffectTexture() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer*	m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

private:
	_float m_fFrame = { 0.0f };
	_vector m_vPosition;

	_uint iCnt = 0;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* 원형객체를 생성하낟.(추후 사본객체가 복사하기위한 대상으로서)  */
	static PlayerRageEffectTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성하낟. */
	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END