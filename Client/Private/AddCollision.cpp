#include "stdafx.h"
#include "..\Public\AddCollision.h"
#include "GameInstance.h"

CAddCollision::CAddCollision(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAddCollision::CAddCollision(const CAddCollision & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAddCollision::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAddCollision::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_Weapon, sizeof m_Weapon);

	if (nullptr != pArg)
		memcpy(&m_Weapon, pArg, sizeof m_Weapon);

	m_eCollisionState = OBJ_NO_COLL;

	m_iObjID = 12;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CAddCollision::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CAddCollision::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_matrix		ParentMatrix = XMLoadFloat4x4(&m_Weapon.pBonePtr->Get_CombinedTransformMatrix()) *
		XMLoadFloat4x4(&m_Weapon.matParentLocal);

	ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
	ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
	ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix * m_Weapon.pParentTransform->Get_WorldMatrix());
}

HRESULT CAddCollision::Render()
{
#ifdef _DEBUG

	//if (nullptr != m_pColliderCom)
	//	m_pColliderCom->Render();

#endif
	return S_OK;
}

void CAddCollision::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		break;
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_MONSTER_WEAPONL:
		break;
	case Engine::OBJ_MONSTER_WEAPONR:
		break;
	case Engine::OBJ_MONSTER_BODY:
		break;
	case Engine::OBJ_MONSTER_BALL:
		break;
	case Engine::OBJ_MONSTER_SICKLE:
		break;
	case Engine::OBJ_MONSTER_BLADE:
		break;
	case Engine::OBJ_NO_COLL:
		break;
	case Engine::OBJ_MONSTER_PROJECTILE:
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CAddCollision::EnterCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		break;
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_MONSTER_WEAPONL:
		break;
	case Engine::OBJ_MONSTER_WEAPONR:
		break;
	case Engine::OBJ_MONSTER_BODY:
		break;
	case Engine::OBJ_MONSTER_BALL:
		break;
	case Engine::OBJ_MONSTER_SICKLE:
		break;
	case Engine::OBJ_MONSTER_BLADE:
		break;
	case Engine::OBJ_NO_COLL:
		break;
	case Engine::OBJ_MONSTER_PROJECTILE:
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

HRESULT CAddCollision::Add_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CAddCollision * CAddCollision::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAddCollision* pInstance = new CAddCollision(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAddCollision");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAddCollision::Clone(void * pArg)
{
	CAddCollision* pInstance = new CAddCollision(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAddCollision");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAddCollision::Free()
{
	__super::Free();

	if (true == m_isCloned)
		Safe_Release(m_Weapon.pBonePtr);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
}
