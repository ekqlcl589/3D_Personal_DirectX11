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

class CGolem_Particle final : public CGameObject
{
protected:
	CGolem_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGolem_Particle(const CGolem_Particle& rhs);
	virtual ~CGolem_Particle() = default;

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
	_vector m_vPosition; // 생성될 위치 
	_vector m_vTarget; // 생성된 후 모여들 위치 

	_bool m_bStart = false;

	_float m_fTime = 3.f; // 생성 후 n초 후 한 점에 모여들기 시작 

	_bool m_isDead = false;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CGolem_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END