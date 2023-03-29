#include "stdafx.h"
#include "..\Public\MonsterBlade.h"
#include "GameInstance.h"

CMonsterBlade::CMonsterBlade(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterBlade::CMonsterBlade(const CMonsterBlade & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterBlade::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBlade::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	memcpy(&m_fPosition, pArg, sizeof(_float3));
	m_fPosition.y = 1.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPosition));

	m_eCollisionState = OBJ_MONSTER_BLADE;

	m_iObjID = 11;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMonsterBlade::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Go_Straight(0.9 * TimeDelta);

}

void CMonsterBlade::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CMonsterBlade::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

		//m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

void CMonsterBlade::OnCollision(CGameObject * pObj)
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CMonsterBlade::EnterCollision(CGameObject * pObj)
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
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

_bool CMonsterBlade::FadeInOut()
{
	return _bool();
}

void CMonsterBlade::Set_Transform()
{
}

HRESULT CMonsterBlade::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Blade"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBlade::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMonsterBlade * CMonsterBlade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterBlade*		pInstance = new CMonsterBlade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBlade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterBlade::Clone(void * pArg)
{
	CMonsterBlade*		pInstance = new CMonsterBlade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMonsterBlade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBlade::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
