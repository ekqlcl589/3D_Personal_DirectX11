#include "stdafx.h"
#include "..\Public\MonsterName.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "Layer.h"
#include "Ancient_StonGolem.h"
#include "GianticCreature.h"
#include "GrudgeWraith.h"
#include "CursedWraith.h"

#include "TargetCamera.h"

CMonsterName::CMonsterName(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMonsterName::CMonsterName(const CMonsterName & rhs)
	: CUI(rhs)
{
}

HRESULT CMonsterName::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterName::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_eOwner, pArg, sizeof OWNER);

	//m_eOwner = OWNER_WRAITH2;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(m_fSizeX, m_fSizeY, 1.f)); // 화면 전체를 덮지만 글자 빼고 알파 0이라 ㄱㅊ? 

	XMStoreFloat4x4(&m_ViewMatrix,
		XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));


	return S_OK;
}

void CMonsterName::Tick(_double TimeDelta)
{
	if (Dead)
	{
		return;

	}
	else
	{
		__super::Tick(TimeDelta);

		if (m_bRender)
			m_fTimeder -= 1.0 * TimeDelta;

		if (m_fTimeder <= 0.0)
		{
			m_fTimeder = 0.0;

			XMStoreFloat4x4(&m_WorldMatrix,
				XMMatrixScaling(m_fSizeX - 400.f, m_fSizeY - 200.f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + 730.f, 0.f));

		}
		_float VertexHpY = 0.f;
		_float TexHpY = 0.f;

		CGameInstance* p = GET_INSTANCE(CGameInstance);
		CGameObject* pMonster = nullptr;
		CGameObject* pCamera = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));

		if (nullptr == p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
		{
			Set_Dead();
			return RELEASE_INSTANCE(CGameInstance);
		}

		pMonster = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		// 몬스터가 죽었으면 Find 하지 말라고 해야 함
		if (m_eOwner == OWNER_GOLEM)
		{
			MaxHP = static_cast<CAncient_StonGolem*>(pMonster)->Get_Info()._MaxHp;
			HP = static_cast<CAncient_StonGolem*>(pMonster)->Get_Info()._Hp;
			Dead = static_cast<CAncient_StonGolem*>(pMonster)->Get_Dead();
		}
		else if (m_eOwner == OWNER_CREATURE)
		{
			MaxHP = static_cast<CGianticCreature*>(pMonster)->Get_Info()._MaxHp;
			HP = static_cast<CGianticCreature*>(pMonster)->Get_Info()._Hp;
			Dead = static_cast<CGianticCreature*>(pMonster)->Get_Dead();
		}

		else if (m_eOwner == OWNER_WRAITH)
		{
			MaxHP = static_cast<CGrudgeWraith*>(pMonster)->Get_Info()._MaxHp;
			HP = static_cast<CGrudgeWraith*>(pMonster)->Get_Info()._Hp;
			Dead = static_cast<CGrudgeWraith*>(pMonster)->Get_Dead();
		}
		else if (m_eOwner == OWNER_WRAITH2)
		{
			MaxHP = static_cast<CCursedWraith*>(pMonster)->Get_Info()._MaxHp;
			HP = static_cast<CCursedWraith*>(pMonster)->Get_Info()._Hp;
			Dead = static_cast<CCursedWraith*>(pMonster)->Get_Dead();
		}


		if (HP > MaxHP)
			HP = MaxHP;

		if (HP <= 0)
		{
			HP = 0;
		}

		m_bRender = static_cast<CTargetCamera*>(pCamera)->Get_RenderNmae();

		RELEASE_INSTANCE(CGameInstance);

		if (Dead)
		{
			m_bRender = false;
			Set_Dead();
		}

	}

}

void CMonsterName::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

}

HRESULT CMonsterName::Render()
{
	if (m_bRender)
	{
		if (FAILED(SetUp_ShaderResource()))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		if (FAILED(__super::Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterName::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MonsterName"),
		TEXT("Com_Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterName::SetUp_ShaderResource()
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

	//float uiNmae = 1.f;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_HpData", &uiNmae, sizeof(float))))
	//	return E_FAIL;

	return S_OK;
}

CMonsterName * CMonsterName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterName * pInstance = new CMonsterName(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("HPBar Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterName::Clone(void * pArg)
{
	CMonsterName * pInstance = new CMonsterName(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("HPBar Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterName::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}