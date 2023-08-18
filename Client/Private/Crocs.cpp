#include "stdafx.h"
#include "..\Public\Crocs.h"

#include "GameInstance.h"

CCrocs::CCrocs(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCrocs::CCrocs(const CCrocs & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCrocs::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrocs::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_HairParts, sizeof m_HairParts);

	if (nullptr != pArg)
		memcpy(&m_HairParts, pArg, sizeof m_HairParts);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-40.0f));

	return S_OK;
}

void CCrocs::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CCrocs::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	//XMLoadFloat4x4(&m_HairParts.pBonePtr->Get_OffsetMatrix()) *
	_matrix		ParentMatrix = 
		XMLoadFloat4x4(&m_HairParts.pBonePtr->Get_CombinedTransformMatrix()) *
		XMLoadFloat4x4(&m_HairParts.matParentLocal);

	ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
	ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
	ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix * m_HairParts.pParentTransform->Get_WorldMatrix());
}

HRESULT CCrocs::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

		//m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CCrocs::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Crocs"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrocs::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CCrocs * CCrocs::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrocs*		pInstance = new CCrocs(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrocs");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrocs::Clone(void * pArg)
{
	CCrocs*		pInstance = new CCrocs(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrocs");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrocs::Free()
{
	__super::Free();

	if (true == m_isCloned)
		Safe_Release(m_HairParts.pBonePtr);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
