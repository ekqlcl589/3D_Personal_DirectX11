#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;
	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual	HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual	HRESULT Initialize(void* pArg) override;

public:
	/* _bool : 움직일수 있어. 없어. */
	/*       : 움직일수 있어. 없어. 슬라이딩을 해야해 */
	_bool Move_OnNavigation(_fvector vPosition /* 움직이고난 결과위치*/ );

#ifdef _DEBUG
public:
//	HRESULT Render();
#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

private: /* 이 네비게이션을 이용하는 객체가 존재하는 쎌의 인덱스 */
	NAVIDESC						m_NaviDesc;

#ifdef _DEBUG
private:
//	class CShader*			m_pShader = { nullptr };
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END