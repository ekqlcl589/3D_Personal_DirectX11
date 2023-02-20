#include "stdafx.h"
#include "..\Public\StaticMesh.h"

#include "GameInstance.h"
#include "ImGui_Tool.h"

CStaticMesh::CStaticMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStaticMesh::CStaticMesh(const CStaticMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStaticMesh::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticMesh::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	ZeroMemory(&m_MeshState, sizeof MESHSTATE);

	memcpy(&m_MeshState, &pArg, sizeof(_tchar));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_MeshState.fPos));

	m_pTransformCom->Set_Scale(m_MeshState.fScale);

	//m_pTransformCom->Rotation(XMMatrixRotationAxis(),m_MeshState.transformDesc.fRotation)

	m_MeshState.iMeshNum = CImGui_Tool::iTileNum++;

	m_MeshState.m_ChangeKey; // arg에서 넘겨 받은 값 그니까 여기서는 뭘 안 해줘도 될 듯?

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CStaticMesh::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CStaticMesh::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CStaticMesh::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CStaticMesh::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_MeshState.m_ChangeKey,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticMesh::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = pInstance->Get_Light(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CStaticMesh * CStaticMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticMesh*		pInstance = new CStaticMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStaticMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticMesh::Clone(void * pArg)
{
	CStaticMesh*		pInstance = new CStaticMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CStaticMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticMesh::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
