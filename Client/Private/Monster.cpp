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
	, m_pTarget(rhs.m_pTarget)
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

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pTarget = static_cast<CPlayer_Body*>(pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

	m_pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	RELEASE_INSTANCE(CGameInstance);

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

	ChaseToPlayer();
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

void CMonster::OnCollisionEnter()
{
}

void CMonster::ChaseToPlayer()
{
	if (false == m_bCheck)
	{

		m_vTargetPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);// -m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_fTagetDistance = 5.f;

		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= m_fTagetDistance)
			m_bPlayerChecck = true;
		else
			m_bPlayerChecck = false;
	}
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
