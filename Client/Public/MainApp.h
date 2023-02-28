#pragma once

#include "Client_Defines.h"
#include "base.h"	

BEGIN(Engine)
class CGameInstance;
class CRenderer;

END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	HRESULT Open_Level(LEVELID eLevelID);
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Prototype_GameObject_ForStatic();
	HRESULT Ready_Fonts();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

public:
	static CMainApp* Create();

public:
	virtual void Free()override;
};

END