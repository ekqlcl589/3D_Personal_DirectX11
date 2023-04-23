#include "stdafx.h"
#include "..\Public\ProjectileSton.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "MonsterWeapon.h"

_bool CProjectileSton::m_bThrow = false;

CProjectileSton::CProjectileSton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectileSton::CProjectileSton(const CProjectileSton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectileSton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectileSton::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eCollisionState = OBJ_MONSTER_PROJECTILE;

	m_iObjID = 13;


	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	m_LifeTime = 4.0;

	m_bDead = false;

	return S_OK;
}

void CProjectileSton::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pTarget = static_cast<CTSPlayer*>(pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

		CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

		_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		RELEASE_INSTANCE(CGameInstance);

		__super::Tick(TimeDelta);

		m_LifeTime -= 1.0 * TimeDelta;

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(XMLoadFloat4x4(&m_matWorldMatrix));

		if (m_bThrow)
		{
			m_RealThrow = true;
		}

		_matrix World = XMLoadFloat4x4(&m_matWorldMatrix);

		_vector vPos = World.r[3];
		_vector	vLook = World.r[2];

		_vector vDir = vTargetPos - XMLoadFloat4x4(&m_matWorldMatrix).r[3];

		if (m_RealThrow)
		{
			vPos += XMVector3Normalize(vDir) * 1.f; //  타임 델타 곱하고.. 속도 주고...
			vPos = XMVectorSetW(vPos, 1.f);

			World = XMLoadFloat4x4(&m_matWorldMatrix);
			World.r[3] = vPos;

			XMStoreFloat4x4(&m_matWorldMatrix, World);


		}

		if (m_LifeTime <= 0.0)
		{
			m_bThrow = false;
			m_RealThrow = false;
			m_LifeTime = 0.0;
			Set_Dead(); 
		}

	}
}

void CProjectileSton::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (!m_bThrow)
			XMStoreFloat4x4(&m_matWorldMatrix, m_pTransformCom->Get_WorldMatrix() * CMonsterWeapon::WorldMatrix);
		
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CProjectileSton::Render()
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

#ifdef _DEBUG

	//if (nullptr != m_pColliderCom)
	//	m_pColliderCom->Render();

#endif


	return S_OK;
}

void CProjectileSton::OnCollision(CGameObject * pObj)
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CProjectileSton::EnterCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_MONSTER_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"EFF_MON_Common_Attack_01_B.ogg", SOUND_MONSTER_EFFECT, 1.0f);

		Set_Dead();
	}
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

HRESULT CProjectileSton::Add_Components()
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

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Projectile"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectileSton::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_matWorldMatrix)))
		return E_FAIL;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CProjectileSton * CProjectileSton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CProjectileSton* pInstance = new CProjectileSton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectileSton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CProjectileSton::Clone(void * pArg)
{
	CProjectileSton* pInstance = new CProjectileSton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectileSton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectileSton::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
