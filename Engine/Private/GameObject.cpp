#include "..\Public\GameObject.h"

#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned{ false }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned{ true }
	, m_eCollisionState(rhs.m_eCollisionState)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->OnCollision(OBJ_PLAYER, OBJ_BOSS1); // 플레이어와 보스 충돌 
	pGameInstance->OnCollision(OBJ_PLAYER, OBJ_MONSTER_WEAPONL); // 플레이어와 보스 왼 팔 충돌 
	pGameInstance->OnCollision(OBJ_PLAYER, OBJ_MONSTER_WEAPONR); // 플레이어와 보스 오른 팔 충돌 
	pGameInstance->OnCollision(OBJ_PLAYER, OBJ_MONSTER_BODY); // 플레이어와 보스 몸통 충돌 
 	pGameInstance->OnCollision(OBJ_WEAPON_SS, OBJ_MONSTER_WEAPONL); // 무기와 보스 충돌 
	pGameInstance->OnCollision(OBJ_WEAPON_SS, OBJ_MONSTER_WEAPONR);
	pGameInstance->OnCollision(OBJ_WEAPON_SS, OBJ_MONSTER_BODY);

	pGameInstance->OnCollision(OBJ_BOSS1, OBJ_PLAYER); // 플레이어와 보스 충돌 
	pGameInstance->OnCollision(OBJ_MONSTER_WEAPONL, OBJ_PLAYER); // 플레이어와 보스 왼 팔 충돌 
	pGameInstance->OnCollision(OBJ_MONSTER_WEAPONR, OBJ_PLAYER); // 플레이어와 보스 오른 팔 충돌 
	pGameInstance->OnCollision(OBJ_MONSTER_BODY, OBJ_PLAYER); // 플레이어와 보스 몸통 충돌 

	Safe_Release(pGameInstance);

}

void CGameObject::LateTick(_double TimeDelta)
{

}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::KnockBack(_uint iDamage, _double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);



	RELEASE_INSTANCE(CGameInstance);
}

void CGameObject::Hit(_uint iDamage)
{
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	m_Components.emplace(pComponentTag, pComponent);
	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	return Find_Component(pComponentTag);
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
