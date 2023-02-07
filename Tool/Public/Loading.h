#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

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

public:
	static CLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free()override;
};

END