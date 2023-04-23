#include "stdafx.h"
#include "..\Public\BlackBall.h"
#include "GameInstance.h"
#include "TargetCamera.h"
#include "TSPlayer.h"

CBlackBall::CBlackBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlackBall::CBlackBall(const CBlackBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlackBall::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_BallDesc, sizeof BALLDESC);

	memcpy(&m_BallDesc, pArg, sizeof(BALLDESC));

	_float3 fScale = m_pTransformCom->Get_Scale();
	if (m_BallDesc.eType == TYPE_8)
	{
		m_vPosition = m_BallDesc.vPosition;
		m_vLook = XMVector3Normalize(m_BallDesc.vLook) * -1;
		XMStoreFloat3(&m_fPos, m_vPosition);
		m_fPos.y = 1.5f;
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.0f, 0.f, 0.f, 0.f) * fScale.x);
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.0f, 1.f, 0.f, 0.f) * fScale.y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook * fScale.z));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos)); // 생성시킬 때 y에 1.5 더해줌

		//Add_Effect();
	}
	else if (m_BallDesc.eType == TYPE_3)
	{
		m_vPosition = m_BallDesc.vPosition; // 생성되야 할 위치 

		m_fPos = m_BallDesc.fOriginPos;
		//XMStoreFloat3(&m_fPos, m_vPosition); // 이 그냥 생성되면 땅을 기니까 y에 1.5 +
		m_fPos.y = 1.5f;
		m_vLook = XMVector3Normalize(m_BallDesc.vLook);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.0f, 0.f, 0.f, 0.f)* fScale.x);
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.0f, 1.f, 0.f, 0.f)* fScale.y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook * fScale.z));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos));

		//Add_Effect();
	}
	else if (m_BallDesc.eType == TYPE_DDEBASI)
	{
		m_vPosition = m_BallDesc.vPosition;
		//m_vLook = XMVector3Normalize(m_BallDesc.vLook);

		XMStoreFloat3(&m_fPos, m_vPosition);
		m_fPos.y = 20.f;
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos));

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pTarget = static_cast<CTSPlayer*>(pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player")));

		CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY2, TEXT("Layer_Player"), TEXT("Com_Transform")));

		_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		RELEASE_INSTANCE(CGameInstance);

		m_pTransformCom->LookAt(vTargetPos);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_Sphere2"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		CSoundMgr::GetInstance()->StopSound(SOUND_MONSTER_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"EFF_MON_Attack_DarkMagic_03_A.ogg", SOUND_MONSTER_EFFECT, 0.5f);

	}


	if (m_BallDesc.eType == TYPE_DDEBASI)
	{

	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_Sphere"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	m_bDead = false;
	Dead = false;
	m_LifeTime = 10.f;

	m_eCollisionState = OBJ_MONSTER_BALL;
	m_iObjID = 9;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

void CBlackBall::Tick(_double TimeDelta)
{
	if(!m_bDead)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 fPosition;
		XMStoreFloat3(&fPosition, vPosition);

		m_LifeTime -= TimeDelta * 1.f;

		if (m_LifeTime <= 0.0)
		{
			m_LifeTime = 0.0;
			Set_Dead();
		}

		__super::Tick(TimeDelta);

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		if (m_BallDesc.eType == TYPE_DDEBASI)
		{	

			m_pTransformCom->Go_Straight(TimeDelta * 4.f);

			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pCamera = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Camera"));
			RELEASE_INSTANCE(CGameInstance);

			if(fPosition.y <= 1.f && fPosition.y >= 0.f)
				static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::LOOK, 0.5f, 0.3f);
		}
		else
		{
			if (!m_Hit)
				m_pTransformCom->Go_Straight(TimeDelta * 1.5f);
			else
				m_pTransformCom->Go_Back(TimeDelta * 1.5f);
		}
	}
}

void CBlackBall::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CBlackBall::Render()
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

void CBlackBall::OnCollision(CGameObject * pObj)
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CBlackBall::EnterCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;
	pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player"));
	m_bTakeHit = static_cast<CTSPlayer*>(pPlayer)->Get_Attack();
	RELEASE_INSTANCE(CGameInstance);

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
	{
		if (m_BallDesc.eType == TYPE_DDEBASI)
		{
			CSoundMgr::GetInstance()->StopSound(SOUND_MONSTER_EFFECT);
			CSoundMgr::GetInstance()->SoundPlay(L"EFF_MON_Big_Fireball_01_A.ogg", SOUND_MONSTER_EFFECT, 1.0f);
		}
		
		Set_Dead();
		break;
	}
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		break;
	case Engine::OBJ_WEAPON_KARMA14:
		// 플레이어가 공격중일 때 검 과 볼이 만나면 look을 반사해서 날림 
		if (m_bTakeHit)
			m_Hit = true;
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

HRESULT CBlackBall::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


HRESULT CBlackBall::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	 
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float fDissolve = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAmount", &fDissolve, sizeof(float))))
		return E_FAIL;

	bool m_GlowOff = false;

	if (m_BallDesc.eType != TYPE_DDEBASI)
		m_GlowOff = true;

	return S_OK;
}

HRESULT CBlackBall::Add_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Fire"), TEXT("Fire_Effect"))))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBlackBall * CBlackBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlackBall* pInstance = new CBlackBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBlackBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlackBall::Clone(void * pArg)
{
	CBlackBall* pInstance = new CBlackBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBlackBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackBall::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
