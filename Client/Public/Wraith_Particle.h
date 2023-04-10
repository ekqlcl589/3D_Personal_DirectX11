#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CWraith_Particle final : public CGameObject
{
protected:
	CWraith_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWraith_Particle(const CWraith_Particle& rhs);
	virtual ~CWraith_Particle() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };	
	CTransform*					m_pTransformCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

private:
	_vector m_vPosition;
	_vector m_vTarget; // 생성된 후 모여들 위치 

	_bool m_isRageSkill = false;

	_bool m_isDead = false;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWraith_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END