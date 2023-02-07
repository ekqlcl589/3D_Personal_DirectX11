#pragma once

#include "Base.h"
#include "Timer.h"


BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float		Get_Timer(const _tchar* pTimerTag);
	void		Set_Timer(const _tchar* pTimerTag);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	
private:
	CTimer*		Find_Timer(const _tchar* pTimerTag);
	
private:
	unordered_map<const _tchar*, CTimer*>		m_umapTimers;

public:
	virtual	void Free(void)override;
};

END
