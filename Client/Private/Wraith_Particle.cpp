#include "stdafx.h"
#include "..\Public\Wraith_Particle.h"

#include "GameInstance.h"
#include "GrudgeWraith.h"

CWraith_Particle::CWraith_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)	
	: CGameObject(pDevice, pContext)
{
}

CWraith_Particle::CWraith_Particle(const CWraith_Particle & rhs)	
	: CGameObject(rhs)
{
}


HRESULT CWraith_Particle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWraith_Particle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	memcpy(&m_vPosition, pArg, sizeof(_vector));

	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	//CGameObject* pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	//CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_CameraTransform")));
	//_vector TargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//RELEASE_INSTANCE(CGameInstance);
	m_vTarget = m_vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);
	return S_OK;
}

void CWraith_Particle::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
 		__super::Tick(TimeDelta);

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Monster_JJol"));
		CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY2, TEXT("Layer_Monster_JJol"), TEXT("Com_Transform")));
		//_vector TargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_isRageSkill = static_cast<CGrudgeWraith*>(pPlayer)->Get_Skill();

		RELEASE_INSTANCE(CGameInstance);

		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
		m_pVIBufferCom->RePosition(pPlayerTransform->Get_State(CTransform::STATE_POSITION), TimeDelta);
		//m_pVIBufferCom->FireSparks(pPlayerTransform->Get_State(CTransform::STATE_POSITION), TimeDelta);

		m_isDead = static_cast<CGrudgeWraith*>(pPlayer)->Get_IsParticle();

		if (!m_isDead)
			Set_Dead();
	}
}

void CWraith_Particle::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CWraith_Particle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))	
		return E_FAIL;	

	/* Apply함수를 호출하기전에 내가 사용하고자하는 셰이더 전역변수로 필요한값을 모두 던져야하낟. */
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CWraith_Particle::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), 
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_VIBuffer_Point_Instance_Wraith"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;	

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Texture_Wraith_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CWraith_Particle::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;	
	
	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CWraith_Particle * CWraith_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWraith_Particle*		pInstance = new CWraith_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWraith_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWraith_Particle::Clone(void* pArg)
{
	CWraith_Particle*		pInstance = new CWraith_Particle(*this);

	/* 우ㅏㅓㄴ형데이터 외에 사본에게 필요한 추가 초기화 데이터를 처리한ㄷ.,, */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWraith_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWraith_Particle::Free()
{
	__super::Free();



	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
