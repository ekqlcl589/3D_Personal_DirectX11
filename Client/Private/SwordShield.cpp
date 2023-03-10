#include "stdafx.h"
#include "..\Public\SwordShield.h"
#include "GameInstance.h"

CSwordShield::CSwordShield(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon(pDevice, pContext)
{
}

CSwordShield::CSwordShield(const CSwordShield & rhs)
	: CWeapon(rhs)
{
}

HRESULT CSwordShield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//if (m_Weapon.WeaponType == WEAPON_SHILED)
	//	m_eCollisionState = OBJ_WEAPON_SS1;
	//else
	//{
		m_eCollisionState = OBJ_WEAPON_SS;

	//}

	return S_OK;
}

HRESULT CSwordShield::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_Weapon, sizeof m_Weapon);

	if (nullptr != pArg)
		memcpy(&m_Weapon, pArg, sizeof m_Weapon);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eCollisionState = OBJ_WEAPON_SS;

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.0f));

	if (m_Weapon.WeaponType == WEAPON_SHILED)
		m_pTransformCom->Rotation(XMVectorSet(1.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));

	return S_OK;
}

void CSwordShield::Tick(_double TimeDelta)
{
	if (false == m_bStart)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		if (m_Weapon.WeaponType == WEAPON_SHILED)
			pInstance->Add_Collider(m_eCollisionState, 2, this);
		else
			pInstance->Add_Collider(m_eCollisionState, 1, this);

		RELEASE_INSTANCE(CGameInstance);

		m_bStart = true;

	} // 얘네는 하나가 아

	__super::Tick(TimeDelta);
}

void CSwordShield::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_matrix		ParentMatrix = XMLoadFloat4x4(&m_Weapon.pBonePtr->Get_CombinedTransformMatrix()) *
		XMLoadFloat4x4(&m_Weapon.matParentLocal);

	ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
	ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
	ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix * m_Weapon.pParentTransform->Get_WorldMatrix());
}

HRESULT CSwordShield::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CSwordShield::Add_Components()
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
	/* For.Com_Model */
	if (m_Weapon.WeaponType == WEAPON_SWORD)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_SS"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		CCollider::COLLIDERDESC ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

		ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
			TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
			return E_FAIL;
	}
	else if (m_Weapon.WeaponType == WEAPON_SHILED)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Shield"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		CCollider::COLLIDERDESC ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

		ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
			TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
			return E_FAIL;

	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordShield::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = pGameInstance->Get_Light(0);
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

CWeapon * CSwordShield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSwordShield* pInstance = new CSwordShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SwordShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordShield::Clone(void * pArg)
{
	CSwordShield* pInstance = new CSwordShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwordShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordShield::Free()
{
	__super::Free();

	if (true == m_isCloned)
		Safe_Release(m_Weapon.pBonePtr);

	Safe_Release(m_pColliderCom);
}
