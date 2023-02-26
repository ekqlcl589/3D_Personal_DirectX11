#include "stdafx.h"
#include "..\Public\Ancient_StonGolem.h"


CAncient_StonGolem::CAncient_StonGolem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CAncient_StonGolem::CAncient_StonGolem(const CAncient_StonGolem & rhs)
	: CMonster(rhs)
{
}

HRESULT CAncient_StonGolem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAncient_StonGolem::Initialize(void * pArg)
{
	return S_OK;
}

void CAncient_StonGolem::Tick(_double TimeDelta)
{
}

void CAncient_StonGolem::LateTick(_double TimeDelta)
{
}

HRESULT CAncient_StonGolem::Render()
{
	return S_OK;
}

CAncient_StonGolem * CAncient_StonGolem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CAncient_StonGolem::Clone(void * pArg)
{
	return nullptr;
}

void CAncient_StonGolem::Free()
{
	__super::Free();
}
