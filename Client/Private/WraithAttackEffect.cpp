#include "stdafx.h"
#include "..\Public\WraithAttackEffect.h"
#include "GameInstance.h"
#include "TSPlayer.h"

CWraithAttackEffect::CWraithAttackEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWraithAttackEffect::CWraithAttackEffect(const CWraithAttackEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWraithAttackEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWraithAttackEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(0);

	memcpy(&m_vPosition, pArg, sizeof(_vector));
	

	m_pModelCom->Set_AnimTick(13.0);

	m_fDissolveTime = 3.f;

	m_bActive = true;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	RELEASE_INSTANCE(CGameInstance);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);

	//m_pTransformCom->LookAt(m_vPosition);

	return S_OK;
}

void CWraithAttackEffect::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		if (true == m_pModelCom->Get_AnimFinished())
		{
			m_fDissolveTime -= TimeDelta;

			fDissolveAmount = Lerp(1.f, 0.f, m_fDissolveTime / 3.f);
			m_bActive = false;
			if (m_fDissolveTime <= 0.f)
				Set_Dead();
		}

	}
}

void CWraithAttackEffect::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (m_bActive)
			m_pModelCom->Play_Animation(TimeDelta);

		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}

	}
}

HRESULT CWraithAttackEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DepthTexture", i, aiTextureType_DIFFUSE);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

		m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CWraithAttackEffect::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_WraithAttack"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWraithAttackEffect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAmount", &fDissolveAmount, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

CWraithAttackEffect * CWraithAttackEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWraithAttackEffect*		pInstance = new CWraithAttackEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWraithAttackEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWraithAttackEffect::Clone(void * pArg)
{
	CWraithAttackEffect*		pInstance = new CWraithAttackEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWraithAttackEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWraithAttackEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
