#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLoading final : public CLevel
{
private:
	CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevel);
	virtual void Tick(_double TimeDelta)override;

private:
	LEVELID m_eNextLevelID = { LEVEL_END };
	class CLoader* m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

public:
	static CLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free()override;
};

END