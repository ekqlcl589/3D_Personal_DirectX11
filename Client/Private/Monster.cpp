#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"
#include "Player_Body.h"

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

}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG

	for (_uint i = 0; i < COLLIDER_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

#endif

	return S_OK;
}

void CMonster::Collision_ToPlayer()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CCollider* pPlayerCollider = static_cast<CCollider*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider_AABB")));

	CCollider* pWeaponColiider = static_cast<CCollider*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon"), TEXT("Com_Collider")));
	
	m_pColliderCom[COLLIDER_AABB]->Collision(pPlayerCollider);
	m_pColliderCom[COLLIDER_AABB]->Collision(pWeaponColiider);

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::ChaseToPlayer()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector fDist = pPlayerTransform->Get_WorldMatrix().r[3];

	XMVector3Length(fDist);

	RELEASE_INSTANCE(CGameInstance);
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
