#include "stdafx.h"
#include "..\Public\PlayerRageAddEffect.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "TwoHandedSword.h"

CPlayerRageAddEffect::CPlayerRageAddEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerRageAddEffect::CPlayerRageAddEffect(const CPlayerRageAddEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerRageAddEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerRageAddEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(70.0f));

	return S_OK;
}

void CPlayerRageAddEffect::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

		RELEASE_INSTANCE(CGameInstance);

		//m_bActive = static_cast<CTSPlayer*>(pPlayer)->Get_RageState();

		//if (!m_bActive)
		//{
		//	Set_Dead();
		//
		//}

	}

}

void CPlayerRageAddEffect::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}
	}
}

HRESULT CPlayerRageAddEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NoiseTexture", i, aiTextureType_NORMALS);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayerRageAddEffect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Rage_Add_Effect"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerRageAddEffect::SetUp_ShaderResources()
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

	return S_OK;
}

CPlayerRageAddEffect * CPlayerRageAddEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerRageAddEffect*		pInstance = new CPlayerRageAddEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerRageAddEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerRageAddEffect::Clone(void * pArg)
{
	CPlayerRageAddEffect*		pInstance = new CPlayerRageAddEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerRageAddEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerRageAddEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
