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
	/* _bool : �����ϼ� �־�. ����. */
	/*       : �����ϼ� �־�. ����. �����̵��� �ؾ��� */
	_bool Move_OnNavigation(_fvector vPosition /* �����̰� �����ġ*/ );

#ifdef _DEBUG
public:
//	HRESULT Render();
#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

private: /* �� �׺���̼��� �̿��ϴ� ��ü�� �����ϴ� ���� �ε��� */
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