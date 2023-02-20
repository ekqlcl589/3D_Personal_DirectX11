#pragma once

#include "Engine_Defines.h"
#include "Tool_Defines.h"
#include "Base.h"
#include "Cube.h"
#include "Calulator.h"
#include "Monster.h"
#include "TestTile.h"

BEGIN(Engine)

class CVIBuffer_Terrain;
class CTransform;
class CGameInstance;

END


BEGIN(Tool)
class CImGui_Tool final : public CBase 
{
	DECLARE_SINGLETON(CImGui_Tool)
private:
	CImGui_Tool();
	virtual ~CImGui_Tool() = default;

public:
	void Initialize_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick_ImGui(_double TimeDelta);
	void Render_ImGui();


public:
	void Setting_Terrain();
	void Create_Terrain();
	HRESULT Delete_Terrain();
	void LoadTerrain();
	HRESULT Create_Cube();

public:
	_bool Picking();
	_bool MonsterPicking();
	_bool TilePicking();

	_bool MeshCreate();

public:
	void ObjectSetting();

public:
	void Delete_Object();

private:
	HRESULT Open_Level(LEVELID eLevelID);
	
public:
	void SaveData();
	void LoadData();
	void MonsterSave();
	void MonsterLoad();
	void TileSave();
	void TileLoad();

	void StaticMeshSave();
	void StaticMeshLoad();

public:
	static int iCubeNum;
	static int iMonsterNum;
	static int iTileNum;

private:
	CCube::CUBESTATE m_CubeState;
	vector<CCube::CUBESTATE> m_vecCubeData;

	_vector m_vCubePos = { 0.f, 0.f, 0.f };
	_float3 fCubePosition = { 0.f, 0.f, 0.f };
	_float3 fCubeScale = { 1.f, 1.f, 1.f };
	_bool m_bCheck = false;

private:
	vector<CMonster::MONSTERSTATE> m_vecMonsterData;

	_vector m_vMonsterPos = { 0.f, 0.f, 0.f };
	_float3 fMonPosition = { 0.f, 0.f, 0.f };
	_float3 fMonScale = { 1.f, 1.f, 1.f };
	_bool m_bMonster = false;
private:
	vector<CTestTile::TILESTATE> m_vecTileData;

	_vector m_vTilePos = { 0.f, 0.f, 0.f };
	_float3 fTilePosition = { 0.f, 0.f, 0.f };
	_float3 fTileScale = { 1.f, 1.f, 1.f };
	_bool m_bTile = false;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	CCalculator* m_pCaculator = nullptr;
	CVIBuffer_Terrain* m_pBuffer_Terain = nullptr;
	CTransform* m_pTransform = nullptr;

public:
	virtual void Free()override;
};

END