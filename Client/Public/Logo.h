#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLogo final : public CLevel
{
private:
	CLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLogo() = default;

public:
	virtual HRESULT Initialize()override;
	virtual void Tick(_double TimeDelta)override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

public:
	static CLogo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free()override;
};

END