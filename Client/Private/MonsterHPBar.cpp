#include "stdafx.h"
#include "..\Public\MonsterHPBar.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "Layer.h"
#include "Ancient_StonGolem.h"
#include "GianticCreature.h"
#include "GrudgeWraith.h"

CMonsterHPBar::CMonsterHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMonsterHPBar::CMonsterHPBar(const CMonsterHPBar & rhs)
	: CUI(rhs)
{
}

HRESULT CMonsterHPBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHPBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_eOwner, pArg, sizeof OWNER);

	m_eOwner = OWNER_WRAITH;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(700.f, 20.f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + 680.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix,
		XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));


	return S_OK;
}

void CMonsterHPBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CMonsterHPBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_float VertexHpY = 0.f;
	_float TexHpY = 0.f;
	
	CGameInstance* p = GET_INSTANCE(CGameInstance);
	CGameObject* pMonster = nullptr;
	
	if (nullptr == p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
		return RELEASE_INSTANCE(CGameInstance);

	pMonster = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	// 몬스터가 죽었으면 Find 하지 말라고 해야 함
	if (m_eOwner == OWNER_GOLEM)
	{
		MaxHP = static_cast<CAncient_StonGolem*>(pMonster)->Get_Info()._MaxHp;
		HP = static_cast<CAncient_StonGolem*>(pMonster)->Get_Info()._Hp;
	}
	else if (m_eOwner == OWNER_CREATURE)
	{
		MaxHP = static_cast<CGianticCreature*>(pMonster)->Get_Info()._MaxHp;
		HP = static_cast<CGianticCreature*>(pMonster)->Get_Info()._Hp;
	}

	//if (m_eOwner == OWNER_WRAITH && true == pMonster->Get_Dead())
	//{
	//	MaxHP = static_cast<CGrudgeWraith*>(pMonster)->Get_Info()._MaxHp;
	//	HP = static_cast<CGrudgeWraith*>(pMonster)->Get_Info()._Hp;
	//}

	if (HP > MaxHP)
		HP = MaxHP;

	if (HP <= 0)
		HP = 0;

	TexHpY = 0.5f - abs(HP / MaxHP);
	VertexHpY = (-TexHpY);

	

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMonsterHPBar::Render()
{
	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHPBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_ShaderHP"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MonsterHPBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHPBar::SetUp_ShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	float uiHp = HP * 0.001f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_HpData", &uiHp, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

CMonsterHPBar * CMonsterHPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterHPBar * pInstance = new CMonsterHPBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("HPBar Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterHPBar::Clone(void * pArg)
{
	CMonsterHPBar * pInstance = new CMonsterHPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("HPBar Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterHPBar::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}