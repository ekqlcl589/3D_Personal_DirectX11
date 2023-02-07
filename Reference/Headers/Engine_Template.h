#ifndef Template_h__
#define Template_h__

namespace Engine
{
	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}



	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long  Safe_Release(T& pointer)
	{
		unsigned long 	dwRefCnt = 0;

		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();

			if (0 == dwRefCnt)
				pointer = nullptr;
		}
		return dwRefCnt;
	}

	template <typename T>			// ���� �ڷ���, ����ü ������ ����
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>			// ���� �迭�� �����ϴ� �뵵
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor �Լ���ü//////////////////////////

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() = default;

	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = nullptr;
	};
}

#endif // Template_h__
