#include "stdafx.h"
#include "..\Public\PlayerComboReady.h"
#include "GameInstance.h"
#include "TSPlayer.h"

CPlayerComboReady::CPlayerComboReady(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerComboReady::CPlayerComboReady(const CPlayerComboReady & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerComboReady::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerComboReady::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_ReadyDesc, pArg, sizeof(READYEFFECT));

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ReadyDesc.vPosition);

	return S_OK;
}

void CPlayerComboReady::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		CGameInstance* p = GET_INSTANCE(CGameInstance);
		CGameObject* pOwner = nullptr;

		pOwner = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_bActive = static_cast<CTSPlayer*>(pOwner)->Get_Info().SpecialAttack1;

		RELEASE_INSTANCE(CGameInstance);
		if (m_ReadyDesc.eType == TYPE_1)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.0 * TimeDelta);
		if (m_ReadyDesc.eType == TYPE_2)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.5 * TimeDelta);
		if (m_ReadyDesc.eType == TYPE_3)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 4.0 * TimeDelta);
		if (m_ReadyDesc.eType == TYPE_4)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 4.5 * TimeDelta);

		if (!m_bActive)
			Set_Dead();
	}
}

void CPlayerComboReady::LateTick(_double TimeDelta)
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

HRESULT CPlayerComboReady::Render()
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

		//m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayerComboReady::Add_Components()
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

	if (m_ReadyDesc.eType == TYPE_1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComboReady1"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_ReadyDesc.eType == TYPE_2)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComboReady2"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_ReadyDesc.eType == TYPE_3)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComboReady3"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_ReadyDesc.eType == TYPE_4)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComboReady4"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerComboReady::SetUp_ShaderResources()
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

CPlayerComboReady * CPlayerComboReady::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerComboReady*		pInstance = new CPlayerComboReady(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerComboReady");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerComboReady::Clone(void * pArg)
{
	CPlayerComboReady*		pInstance = new CPlayerComboReady(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerComboReady");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerComboReady::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
