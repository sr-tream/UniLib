#pragma once
#include <Windows.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

class CMemCpy
{
public:
	CMemCpy(void *dest, const void *src, size_t stLen, bool needRestore = true)
	{
		// Выделяем память под оригинальные байты
		_backup = new byte[stLen];
		// Запоминаем оригинальные байты
		memcpy_safe(_backup, dest, stLen);
		// Записываем новые байты
		memcpy_safe(dest, src, stLen);
		// Сохраняем параметры необходимые для деструктора
		_dest = dest;
		_stLen = stLen;
		_needRestore = needRestore;
	}
	CMemCpy(DWORD dest, const void *src, size_t stLen, bool needRestore = true)
	{
		_backup = new byte[stLen];
		memcpy_safe(_backup, (void*)dest, stLen);
		memcpy_safe((void*)dest, src, stLen);
		_dest = (void*)dest;
		_stLen = stLen;
		_needRestore = needRestore;
	}
	CMemCpy(void *dest, DWORD src, size_t stLen, bool needRestore = true)
	{
		_backup = new byte[stLen];
		memcpy_safe(_backup, dest, stLen);
		memcpy_safe(dest, (void*)src, stLen);
		_dest = dest;
		_stLen = stLen;
		_needRestore = needRestore;
	}
	CMemCpy(DWORD dest, DWORD src, size_t stLen, bool needRestore = true)
	{
		_backup = new byte[stLen];
		memcpy_safe(_backup, (void*)dest, stLen);
		memcpy_safe((void*)dest, (void*)src, stLen);
		_dest = (void*)dest;
		_stLen = stLen;
		_needRestore = needRestore;
	}

	int CmpDest(void *mem)
	{
		return memcmp_safe(_dest, mem, _stLen);
	}
	int CmpDest(DWORD mem)
	{
		return memcmp_safe(_dest, (void*)mem, _stLen);
	}

	int CmpSrc(void *mem)
	{
		return memcmp_safe(_backup, mem, _stLen);
	}
	int CmpSrc(DWORD mem)
	{
		return memcmp_safe(_backup, (void*)mem, _stLen);
	}

	virtual ~CMemCpy()
	{
		// Восстанавливаем оригинальные байты
		if (_needRestore)
			memcpy_safe(_dest, _backup, _stLen);
		// Удаляем выделеную память
		delete[] _backup;
	}
protected:
	void *memcpy_safe(void *dest, const void *src, size_t stLen)
	{
		if (dest == nullptr || src == nullptr || stLen == 0)
			return nullptr;

		MEMORY_BASIC_INFORMATION	mbi;
		VirtualQuery(dest, &mbi, sizeof(mbi));
		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);

		void	*pvRetn = memcpy(dest, src, stLen);
		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect);
		FlushInstructionCache(GetCurrentProcess(), dest, stLen);
		return pvRetn;
	}
private:
	void *_dest;
	size_t _stLen;
	byte* _backup;

	bool _needRestore;

	int memcmp_safe(const void *_s1, const void *_s2, uint32_t len)
	{
		const uint8_t	*s1 = (const uint8_t *)_s1;
		const uint8_t	*s2 = (const uint8_t *)_s2;
		uint8_t			buf[4096];

		for (;;)
		{
			if (len > 4096)
			{
				if (!memcpy_safe(buf, s1, 4096))
					return 0;
				if (memcmp(buf, s2, 4096))
					return 0;
				s1 += 4096;
				s2 += 4096;
				len -= 4096;
			}
			else
			{
				if (!memcpy_safe(buf, s1, len))
					return 0;
				if (memcmp(buf, s2, len))
					return 0;
				break;
			}
		}

		return 1;
	}
};
class CMemSet : public CMemCpy
{
public:
	CMemSet(void *dest, int c, size_t Len, bool needRestore = true) : CMemCpy(dest, nullptr, Len, needRestore)
	{
		memset_safe(dest, c, Len);
	}
	CMemSet(DWORD dest, int c, size_t Len, bool needRestore = true) : CMemCpy(dest, nullptr, Len, needRestore)
	{
		memset_safe((void*)dest, c, Len);
	}
private:
	int memset_safe(void *_dest, int c, DWORD len)
	{
		byte *dest = (byte *)_dest;
		byte buf[4096];
		memset(buf, c, (len > 4096) ? 4096 : len);
		for (;;)
		{
			if (len > 4096)
			{
				if (!memcpy_safe(dest, buf, 4096))
					return 0;
				dest += 4096;
				len -= 4096;
			}
			else
			{
				if (!memcpy_safe(dest, buf, len))
					return 0;
				break;
			}
		}
		return 1;
	}
};