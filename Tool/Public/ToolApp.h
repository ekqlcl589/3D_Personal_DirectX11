#pragma once

#include "Tool_Defines.h"
#include "base.h"	

BEGIN(Engine)
class CGameInstance;
class CRenderer;

END

BEGIN(Tool)

class CToolApp final : public CBase
{
private:
	CToolApp();
	virtual ~CToolApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	HRESULT Open_Level(LEVELID eLevelID);
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Prototype_GameObject_ForStatic();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

	class CImGui_Tool* m_pTool = { nullptr };
public:
	static CToolApp* Create();

public:
	virtual void Free()override;
};

END