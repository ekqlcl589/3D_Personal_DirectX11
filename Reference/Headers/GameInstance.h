#pragma once

#include "Component_Mgr.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
public:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, const GRAPHIC_DESC& GraphicDesc, _uint iNumLevels, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	HRESULT Tick_Engine(_double TimeDelta);

public: /* Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* Level_Mgr */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pCurrentLevel);

public: /* Obj_Mgr */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);

	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For PipeLine*/
	_float4x4 Get_Transformfloat4x4(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transformfloat4x4Inverse(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_matrix	Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState);
	_float4 Get_CamPos();

public: // For Timer_Mgr
	_float	Get_Timer(const _tchar* pTimerTag);
	void Set_Timer(const _tchar* pTimerTag);
	HRESULT	Ready_Timer(const _tchar* pTimerTag);

public: // For InputDev
	_byte Get_DIKeyState(_ubyte byKeyID);
	_byte Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMoveState);

public: // For Light_Mgr
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_Light(_uint iIndex);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Mgr* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CComponent_Mgr* m_pComponent_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CTimer_Manager* m_pTime_Mgr = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CLight_Mgr* m_pLight_Mgr = { nullptr };

public:
	static void Release_Engine();

public:
	virtual void Free()override;
};

END