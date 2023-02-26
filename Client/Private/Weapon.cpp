#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	for (_uint i = 0; i < COLLIDER_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Update(XMLoadFloat4x4(&m_WorldMatrix));
	}

}

void CWeapon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CWeapon::Render()
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

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
