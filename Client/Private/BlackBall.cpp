#include "stdafx.h"
#include "..\Public\BlackBall.h"
#include "GameInstance.h"

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

	if (m_BallDesc.eType == TYPE_8)
	{
		m_vPosition = m_BallDesc.vLook;
		XMStoreFloat3(&m_fPos, m_vPosition);
		m_fPos.y = 1.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_BallDesc.vLook));

	}
	else if (m_BallDesc.eType == TYPE_3)
	{
		m_vPosition = m_BallDesc.vLook;
		m_vLook = XMVector3Normalize(m_BallDesc.vLook);

		XMStoreFloat3(&m_fPos, m_vPosition);
		m_fPos.y = 1.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_vLook * -1 );

	}
	else if (m_BallDesc.eType == TYPE_DDEBASI)
	{
		m_vPosition = m_BallDesc.vLook;
		m_vLook = XMVector3Normalize(m_BallDesc.vLook);

		XMStoreFloat3(&m_fPos, m_vPosition);
		m_fPos.y = 20.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPos));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_vLook * -1);

	}


	if (m_BallDesc.eType == TYPE_DDEBASI)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere2"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.0f));

	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
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
	if (m_bDead)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pInstance->Dleate_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Ball"))))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		m_LifeTime -= TimeDelta * 1.f;

		if (m_LifeTime <= 0.0)
		{
			m_LifeTime = 0.0;
			Dead = true;
			//return;
		}

		__super::Tick(TimeDelta);
		Add_Collied(this);

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		if (m_BallDesc.eType == TYPE_DDEBASI)
		{	

			XMStoreFloat3(&m_fPos, m_vPosition);
			m_pTransformCom->Go_Straight(TimeDelta * 3.f);

		}
		else
			m_pTransformCom->Go_Straight(TimeDelta * 1.5f);
	}
}

void CBlackBall::LateTick(_double TimeDelta)
{
	if (!Dead)
	{
		__super::LateTick(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CBlackBall::Render()
{
	if (!Dead)
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

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();

	#endif


		return S_OK;
	}
}

void CBlackBall::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		Dead = true;
		//Set_Dead();
		// ī�޶� ����ũ true;
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxWave"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
