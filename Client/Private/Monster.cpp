#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_MonsterState, sizeof(MONSTERSTATE));
	
	memcpy(&m_MonsterState, pArg, sizeof m_MonsterState);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_MonsterState.fPos));
	
	m_pModelCom->SetUp_Animation(rand() % 20);

	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	for (_uint i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
	}

}

void CMonster::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	Collision_ToPlayer();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CMonster::Render()
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

		m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	for (_uint i = 0; i < COLLIDER_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

#endif

	return S_OK;
}

HRESULT CMonster::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss0"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::SetUp_ShaderResources()
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

void CMonster::Collision_ToPlayer()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CCollider* pPlayerCollider = static_cast<CCollider*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider_AABB")));

	m_pColliderCom[COLLIDER_AABB]->Collision(pPlayerCollider);

	RELEASE_INSTANCE(CGameInstance);
}

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLIDER_END; ++i)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
