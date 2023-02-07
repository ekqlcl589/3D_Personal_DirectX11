#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	CRITICAL_SECTION* Get_CriticalSectionPtr() {
		return &m_CriticalSection;
	}

	LEVELID Get_NextLevelID() const {
		return m_eNextLevelID;
	}

	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	_bool isFinished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(LEVELID eNextLevelID);

	HRESULT Loading_ForLogo();
	HRESULT Loading_ForGamePlay();
	/*
	HRESULT Loading_ForBossRoom();
	HRESULT Loading_ForLoby();
	*/
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CRITICAL_SECTION m_CriticalSection;
	HANDLE m_hThread;
	LEVELID m_eNextLevelID;

	_tchar m_szLoadingText[MAX_PATH] = TEXT("");
	_bool m_isFinished = { false };

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free()override;
};

END