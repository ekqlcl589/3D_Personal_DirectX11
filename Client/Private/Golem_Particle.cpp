#include "stdafx.h"
#include "..\Public\Golem_Particle.h"

#include "GameInstance.h"
#include "Ancient_StonGolem.h"

CGolem_Particle::CGolem_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)	
	: CGameObject(pDevice, pContext)
{
}

CGolem_Particle::CGolem_Particle(const CGolem_Particle & rhs)	
	: CGameObject(rhs)
{
}


HRESULT CGolem_Particle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGolem_Particle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	memcpy(&m_vPosition, pArg, sizeof(_vector)); // ���� �������� �޾ƿͼ� �� �������� �������� x, y, z �� ������ ���� 


	m_fTime = 3.f;

	//m_vTarget = m_vPosition; 

	return S_OK;
}

void CGolem_Particle::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
 		__super::Tick(TimeDelta);

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pMonster = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		CTransform* pTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));
		_vector TargetPos = pTransform->Get_State(CTransform::STATE_POSITION);

		RELEASE_INSTANCE(CGameInstance);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition); �ù�.. �׳� ������ ���� ��Ű�� ������ �𿩵�� �ŷ� ����...
	
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, TargetPos);


		m_fTime -= 1.0 * TimeDelta;

		if (m_fTime <= 0.0)
		{
			m_fTime = 0.0;
			m_bStart = true;
		}

		if (m_bStart)
			m_pVIBufferCom->RePosition(TargetPos, TimeDelta);
		
		m_isDead = static_cast<CAncient_StonGolem*>(pMonster)->Get_isParticle();

		if (!m_isDead)
			Set_Dead();
	}
}

void CGolem_Particle::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CGolem_Particle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))	
		return E_FAIL;	

	/* Apply�Լ��� ȣ���ϱ����� ���� ����ϰ����ϴ� ���̴� ���������� �ʿ��Ѱ��� ��� �������ϳ�. */
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CGolem_Particle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;	

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CGolem_Particle::SetUp_ShaderResources()
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

	float fTime = m_fTime;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTime", &fTime, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

CGolem_Particle * CGolem_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGolem_Particle*		pInstance = new CGolem_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGolem_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGolem_Particle::Clone(void* pArg)
{
	CGolem_Particle*		pInstance = new CGolem_Particle(*this);

	/* �줿�ä��������� �ܿ� �纻���� �ʿ��� �߰� �ʱ�ȭ �����͸� ó���Ѥ�.,, */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGolem_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolem_Particle::Free()
{
	__super::Free();



	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
