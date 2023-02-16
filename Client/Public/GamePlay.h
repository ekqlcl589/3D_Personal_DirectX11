#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CGamePlay final : public CLevel
{
private:
	CGamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGamePlay() = default;

public:
	virtual HRESULT Initialize()override;
	virtual void Tick(_double TimeDelta)override;

	HRESULT Ready_Layer_BackGround(const _tchar * pLayerTag); 
	// 배경이 되는 애들을 추가한 레이어 

	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

	HRESULT Ready_Light();

	HRESULT Ready_UI(const _tchar* pLayerTag);

	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);

	void LoadData(_tchar* szFilePath);
	void LoadMonster(_tchar* szFilePath);
	void LoadMeshTile(_tchar* szFilePath);

public:
	static CGamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free()override;
};

END