#include "stdafx.h"
#include "..\Public\MonsterWeapon.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "Ancient_StonGolem.h"
#include "GianticCreature.h"
#include "GrudgeWraith.h"
#include "CursedWraith.h"

CMonsterWeapon::CMonsterWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterWeapon::CMonsterWeapon(const CMonsterWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterWeapon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_Weapon, sizeof m_Weapon);

	if (nullptr != pArg)
		memcpy(&m_Weapon, pArg, sizeof m_Weapon);

	if (m_Weapon.WeaponType == WEAPON_MONSTER_L)
		m_eCollisionState = OBJ_MONSTER_WEAPONL;
	else if (m_Weapon.WeaponType == WEAPON_MONSTER_R)
		m_eCollisionState = OBJ_MONSTER_WEAPONR;
	else if (m_Weapon.WeaponType == WEAPON_MONSTER_BODY)
		m_eCollisionState = OBJ_MONSTER_BODY; 

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	if (m_Weapon.WeaponType == WEAPON_MONSTER_L)
		pInstance->Add_Collider(m_eCollisionState, 6, this);

	if (m_Weapon.WeaponType == WEAPON_MONSTER_R)
		pInstance->Add_Collider(m_eCollisionState, 7, this);
	if (m_Weapon.WeaponType == WEAPON_MONSTER_BODY)
		pInstance->Add_Collider(m_eCollisionState, 8, this);

	if(m_Weapon.Owner == OWNER_WRAITH)
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.0f));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMonsterWeapon::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pOwner = nullptr;

		if (nullptr == pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
		{
			RELEASE_INSTANCE(CGameInstance);
			return ;
		}

		pOwner = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

		if (m_Weapon.Owner == OWNER_GOLEM)
			Dead = static_cast<CAncient_StonGolem*>(pOwner)->Get_Dead();

		else if (m_Weapon.Owner == OWNER_CREATURE)
			Dead = static_cast<CGianticCreature*>(pOwner)->Get_Dead();

		else if (m_Weapon.Owner == OWNER_WRAITH)
			Dead = static_cast<CGianticCreature*>(pOwner)->Get_Dead();

		else if (m_Weapon.Owner == OWNER_WRAITH2)
			Dead = static_cast<CGianticCreature*>(pOwner)->Get_Dead();

		RELEASE_INSTANCE(CGameInstance);

		if (Dead)
			m_bDead = true;

	}
}

void CMonsterWeapon::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		_matrix		ParentMatrix = XMLoadFloat4x4(&m_Weapon.pBonePtr->Get_CombinedTransformMatrix()) *
			XMLoadFloat4x4(&m_Weapon.matParentLocal);

		ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
		ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
		ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix * m_Weapon.pParentTransform->Get_WorldMatrix());
	}
}

HRESULT CMonsterWeapon::Render()
{
	if (!m_bDead)
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

			//m_pModelCom->Render(i);
		}

#ifdef _DEBUG

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();

#endif

		return S_OK;
	}
}

void CMonsterWeapon::OnCollision(CGameObject * pObj)
{
	if (!m_bDead)
	{

		COLLISIONSTATE eType = pObj->Get_ObjType();

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pPlayer = nullptr;
		pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_bTakeHit = static_cast<CTSPlayer*>(pPlayer)->Get_Attack();
		RELEASE_INSTANCE(CGameInstance);

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
		{
			//if (true == m_bTakeHit)
			//{
			//	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			//	CGameObject* pOwner = nullptr;
			//	CGameObject* pTarget = nullptr;
			//
			//	pOwner = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			//	pTarget = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
			//
			//	m_PlayerRSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rSkill;
			//	m_PlayerFSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().fSkill;
			//	m_PlayerRageSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rageSkill;
			//
			//	if (true == m_PlayerRSkill)
			//		Damage = 30.f;
			//	else if (true == m_PlayerFSkill)
			//		Damage = 45.f;
			//	else if (true == m_PlayerRageSkill)
			//		Damage = 100.f;
			//	else
			//		Damage = 5.f;
			//
			//	if (m_Weapon.Owner == OWNER_GOLEM)
			//	{
			//		_uint Hp = static_cast<CAncient_StonGolem*>(pOwner)->Get_Info()._Hp;
			//		static_cast<CAncient_StonGolem*>(pOwner)->Set_Info(Damage);
			//		cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
			//
			//	}
			//	else if (m_Weapon.Owner == OWNER_CREATURE)
			//	{
			//		_uint Hp = static_cast<CGianticCreature*>(pOwner)->Get_Info()._Hp;
			//		static_cast<CGianticCreature*>(pOwner)->Set_Info(Damage);
			//		cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
			//	}
			//	else if (m_Weapon.Owner == OWNER_WRAITH)
			//	{
			//		_uint Hp = static_cast<CGrudgeWraith*>(pOwner)->Get_Info()._Hp;
			//		static_cast<CGrudgeWraith*>(pOwner)->Set_Info(Damage);
			//		cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
			//	}
			//	else if (m_Weapon.Owner == OWNER_WRAITH2)
			//	{
			//		_uint Hp = static_cast<CCursedWraith*>(pOwner)->Get_Info()._Hp;
			//		static_cast<CCursedWraith*>(pOwner)->Set_Info(Damage);
			//		static_cast<CCursedWraith*>(pOwner)->Set_Hit(true);
			//		cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
			//	}
			//
			//	RELEASE_INSTANCE(CGameInstance)
			//}
			break;
		}
		case Engine::OBJ_BOSS2:
			break;
		case Engine::OBJ_MONSTER_WEAPONL:
			break;
		case Engine::OBJ_MONSTER_WEAPONR:
			break;
		case Engine::OBJ_MONSTER_BODY:
			break;
		case Engine::OBJ_END:
			break;
		default:
			break;
		}
	}
}

void CMonsterWeapon::EnterCollision(CGameObject * pObj)
{
	if (!m_bDead)
	{

		COLLISIONSTATE eType = pObj->Get_ObjType();

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pPlayer = nullptr;
		pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_bTakeHit = static_cast<CTSPlayer*>(pPlayer)->Get_Attack();
		RELEASE_INSTANCE(CGameInstance);

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
		{
			if (true == m_bTakeHit)
			{
				CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pOwner = nullptr;
				CGameObject* pTarget = nullptr;

				if (nullptr == pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
				{
					Set_Dead();
					return;
				}

				pOwner = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
				pTarget = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

				m_PlayerRSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rSkill;
				m_PlayerFSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().fSkill;
				m_PlayerRageSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rageSkill;

				if (true == m_PlayerRSkill)
					Damage = 30.f;
				else if (true == m_PlayerFSkill)
					Damage = 45.f;
				else if (true == m_PlayerRageSkill)
					Damage = 100.f;
				else
					Damage = 10.f;

				if (m_Weapon.Owner == OWNER_GOLEM)
				{
					_uint Hp = static_cast<CAncient_StonGolem*>(pOwner)->Get_Info()._Hp;
					static_cast<CAncient_StonGolem*>(pOwner)->Set_Info(Damage);
					cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;

				}
				else if (m_Weapon.Owner == OWNER_CREATURE)
				{
					_uint Hp = static_cast<CGianticCreature*>(pOwner)->Get_Info()._Hp;
					static_cast<CGianticCreature*>(pOwner)->Set_Info(Damage);
					cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
				}
				else if (m_Weapon.Owner == OWNER_WRAITH)
				{
					_uint Hp = static_cast<CGrudgeWraith*>(pOwner)->Get_Info()._Hp;
					static_cast<CGrudgeWraith*>(pOwner)->Set_Info(Damage);
					cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
				}
				else if (m_Weapon.Owner == OWNER_WRAITH2)
				{
					_uint Hp = static_cast<CCursedWraith*>(pOwner)->Get_Info()._Hp;
					static_cast<CCursedWraith*>(pOwner)->Set_Info(Damage);
					static_cast<CCursedWraith*>(pOwner)->Set_Hit(true);
					cout << "Ä®ÇÑÅ× ¸ÂÀ½" << Hp << endl;
				}

				RELEASE_INSTANCE(CGameInstance)
			}
			break;
		}
		case Engine::OBJ_BOSS2:
			break;
		case Engine::OBJ_MONSTER_WEAPONL:
			break;
		case Engine::OBJ_MONSTER_WEAPONR:
			break;
		case Engine::OBJ_MONSTER_BODY:
			break;
		case Engine::OBJ_END:
			break;
		default:
			break;
		}
	}
}

HRESULT CMonsterWeapon::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_SS"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	if (m_Weapon.Owner == OWNER_WRAITH)
	{

		if (m_Weapon.WeaponType == WEAPON_MONSTER_R)
		{
			CCollider::COLLIDERDESC ColliderDesc;
			ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

			ColliderDesc.vScale = _float3(0.5f, 3.f, 0.5f);
			ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
				TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
				return E_FAIL;
		}

		if (m_Weapon.WeaponType == WEAPON_MONSTER_BODY)
		{
			CCollider::COLLIDERDESC ColliderDesc;
			ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

			ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
			ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);
			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
				TEXT("Com_Collider1"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
				return E_FAIL;
		}
	}
	else if (m_Weapon.Owner == OWNER_WRAITH2)
	{
		CCollider::COLLIDERDESC ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

		ColliderDesc.vScale = _float3(2.f, 1.f, 2.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
			return E_FAIL;

	}
	else
	{
		if (m_Weapon.WeaponType == WEAPON_MONSTER_L || m_Weapon.WeaponType == WEAPON_MONSTER_R)
		{
			CCollider::COLLIDERDESC ColliderDesc;
			ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

			ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
			ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
				TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
				return E_FAIL;
		}
	
		if(m_Weapon.WeaponType == WEAPON_MONSTER_BODY)
		{
			CCollider::COLLIDERDESC ColliderDesc;
			ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

			ColliderDesc.vScale = _float3(5.f, 2.f, 5.f);
			ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);
			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
				TEXT("Com_Collider1"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
				return E_FAIL;

		}
	}


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterWeapon::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMonsterWeapon * CMonsterWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterWeapon* pInstance = new CMonsterWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterWeapon::Clone(void * pArg)
{
	CMonsterWeapon* pInstance = new CMonsterWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterWeapon::Free()
{
	__super::Free();

	if (true == m_isCloned)
		Safe_Release(m_Weapon.pBonePtr);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
