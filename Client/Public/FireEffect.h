#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFireEffect final :
	public CBlendObject
{
private:
	CFireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireEffect(const CFireEffect& rhs);
	virtual ~CFireEffect() = default;

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
	CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };

private:
	_float m_fFrame = { 0.0f };
	_vector m_vPosition;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* ������ü�� �����ϳ�.(���� �纻��ü�� �����ϱ����� ������μ�)  */
	static CFireEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* �纻��ü�� �����ϳ�. */
	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END