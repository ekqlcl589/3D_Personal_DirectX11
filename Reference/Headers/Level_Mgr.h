#pragma once
#include "Level.h"

BEGIN(Engine)

class CLevel_Mgr final : public CBase
{
	DECLARE_SINGLETON(CLevel_Mgr)
private:
	CLevel_Mgr();
	virtual ~CLevel_Mgr() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pCurrentLevel);
	void Tick_Level(_double TimeDelta);

private:
	class CLevel* m_pCurrentLevel = { nullptr };
	_uint m_iLevelIndex = { 0 };

public:
	virtual void Free()override;
};

END