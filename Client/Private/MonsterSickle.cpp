#include "stdafx.h"
#include "..\Public\MonsterSickle.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "GrudgeWraith.h"

CMonsterSickle::CMonsterSickle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterSickle::CMonsterSickle(const CMonsterSickle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterSickle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterSickle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_Weapon, sizeof m_Weapon);

	if (nullptr != pArg)
		memcpy(&m_Weapon, pArg, sizeof m_Weapon);

	m_eCollisionState = OBJ_MONSTER_SICKLE;

	m_iObjID = 10;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.0f));

	return S_OK;
}

void CMonsterSickle::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);
	
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pOwner = nullptr;

		RELEASE_INSTANCE(CGameInstance); // ¿©±â ¿Ö ÅÍÁü..?
		//if (nullptr == pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Monster_JJol")))
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}
		//
		//pOwner = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Monster_JJol"));
		//RELEASE_INSTANCE(CGameInstance);
		//
		//Dead = static_cast<CGrudgeWraith*>(pOwner)->Get_Dead();

		if (Dead)
			Set_Dead();
	}
}

void CMonsterSickle::LateTick(_double TimeDelta)
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

HRESULT CMonsterSickle::Render()
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

		//if (nullptr != m_pColliderCom)
		//	m_pColliderCom->Render();

#endif

		return S_OK;
	}
	return S_OK;
}

void CMonsterSickle::OnCollision(CGameObject * pObj)
{
	if (!m_bDead)
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
		case Engine::OBJ_END:
			break;
		default:
			break;
		}
	}
}

void CMonsterSickle::EnterCollision(CGameObject * pObj)
{
	if (!m_bDead)
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
		case Engine::OBJ_END:
			break;
		default:
			break;
		}
	}
}

HRESULT CMonsterSickle::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_Weapon_SS1"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(0.5f, 3.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterSickle::SetUp_ShaderResources()
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

CMonsterSickle * CMonsterSickle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterSickle* pInstance = new CMonsterSickle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterSickle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterSickle::Clone(void * pArg)
{
	CMonsterSickle* pInstance = new CMonsterSickle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterSickle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterSickle::Free()
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
