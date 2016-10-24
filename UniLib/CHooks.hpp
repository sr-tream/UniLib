#pragma once
#include "CMem.hpp"

// Структура для поиска кода по сигнатуре
struct stFindPatern
{
	DWORD dwAddress;
	DWORD dwLen;
	BYTE *bMask;
	char *szMask;
};

// Перемещает часть указанного кода в виртуальную память и добовляет к нему ваш код, в начало или конец.
class CVirtualCode
{
public:
	CVirtualCode(void* src, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		// Копируем параметры для деструктора
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//Мы не можем поставить команду jmp имеющую код 0xE9 в пространство менее 5 байт
		if (_origLen < 5)
			return;

		// Выделяем память
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// Расчитываем начало оригинального кода
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// Копируем данные в выделеную память
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// Исправляем адреса некоторых инструкций jmp т call
		ModifyCode();
		// Заменяем оригинальные инструкции нопами
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// Расчитываем начало пользовательского кода в памяти
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// Копируем пользовательский код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// Записываем в оригинальный код инструкцию jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// Получаем относительный адрес, для перехода в виртуальную память
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// Записываем инструкцию jmp в конец виртуальной памяти
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// Получаем относительный адрес, для выхода из виртуальной памяти
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpExitAddr = new CMemCpy((DWORD)_virtualAddr + origLen + len + 1, &RelativeAddress, 4, false);
	}
	CVirtualCode(stFindPatern src_base, void* src, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		src = (void*)((DWORD)src + FindPattern(src_base.dwAddress, src_base.dwLen, src_base.bMask, src_base.szMask));
		delete[] src_base.bMask;
		delete[] src_base.szMask;

		// Копируем параметры для деструктора
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//Мы не можем поставить команду jmp имеющую код 0xE9 в пространство менее 5 байт
		if (_origLen < 5)
			return;

		// Выделяем память
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// Расчитываем начало оригинального кода
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// Копируем данные в выделеную память
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// Исправляем адреса некоторых инструкций jmp т call
		ModifyCode();
		// Заменяем оригинальные инструкции нопами
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// Расчитываем начало пользовательского кода в памяти
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// Копируем пользовательский код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// Записываем в оригинальный код инструкцию jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// Получаем относительный адрес, для перехода в виртуальную память
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// Записываем инструкцию jmp в конец виртуальной памяти
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// Получаем относительный адрес, для выхода из виртуальной памяти
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpExitAddr = new CMemCpy((DWORD)_virtualAddr + origLen + len + 1, &RelativeAddress, 4, false);
	}
	CVirtualCode(DWORD TableDx9Id, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		static DWORD* vtbl = 0, table;
		table = FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
		memcpy(&vtbl, (void*)(table + 2), 4);
		void *src = (void*)vtbl[TableDx9Id];

		// Копируем параметры для деструктора
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//Мы не можем поставить команду jmp имеющую код 0xE9 в пространство менее 5 байт
		if (_origLen < 5)
			return;

		// Выделяем память
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// Расчитываем начало оригинального кода
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// Копируем данные в выделеную память
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// Исправляем адреса некоторых инструкций jmp т call
		ModifyCode();
		// Заменяем оригинальные инструкции нопами
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// Расчитываем начало пользовательского кода в памяти
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// Копируем пользовательский код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// Записываем в оригинальный код инструкцию jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// Получаем относительный адрес, для перехода в виртуальную память
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// Записываем инструкцию jmp в конец виртуальной памяти
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// Получаем относительный адрес, для выхода из виртуальной памяти
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// Записываем полученый относительный адрес в качестве операнда инструкции jmp
		asmJmpExitAddr = new CMemCpy((DWORD)_virtualAddr + origLen + len + 1, &RelativeAddress, 4, false);
	}

	void NopOriginalCode(bool nop)
	{
		if (nop && MemSet_orig == nullptr)
			MemSet_orig = new CMemSet(_origAddr, 0x90, _origLen);
		else if ( !nop && MemSet_orig != nullptr ){
			delete MemSet_orig;
			MemSet_orig = NULL;
		}
	}

	virtual ~CVirtualCode()
	{
		if (_origLen < 5)
			return; //Нечего удалять, мы ничего не создавали

		// Код нам больше не нужен - удаляем его класс
		delete MemCpy_code;
		// Восстанавливаем оригинальные инструкции
		NopOriginalCode(false);

		if ( !_SafeVirtual ) {
			// Удаляем не нужные нам классы, что бы не висели в памяти вечно
			delete MemCpy_orig;
			delete asmJmpAddr;
			delete asmJmp;
			delete asmJmpExitAddr;
			delete asmJmpExit;
			// Удаляем нопы из оригинального кода, тем самым восстанавливая его
			delete MemSet;
			// Освобождаем память
			VirtualFree(_virtualAddr, _origLen + _len + 5, MEM_RELEASE);
		} else {
			// Заменяем нопами пользовательский код
			memset((void*)_codeAddr, 0x90, _len);

			// Небольшая оптимизация - перескакиваем нопы
			if (_len > 5){
				*(byte*)_codeAddr = 0xE9; //Jmp
				DWORD RelativeAddress = GetRelativeAddress(_codeAddr, _origAddr);
				memcpy((void*)(_codeAddr + 1), &RelativeAddress, 4);
			}
		}
	}
protected:
	DWORD _codeAddr;
	CMemCpy *MemCpy_code;

	DWORD GetRelativeAddress(DWORD dwFrom /*hook addr*/, DWORD dwTo /*func addr*/)
	{
		return dwTo - (dwFrom + 5);
	}
	bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
			if (*szMask == 'x' && *pData != *bMask)
				return false;

		return (*szMask) == NULL;
	}
	DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
	{
		for (DWORD i = 0; i < dwLen; i++)
			if (bCompare((BYTE*)(dwAddress + i), bMask, szMask))
				return (DWORD)(dwAddress + i);

		return 0;
	}
private:
	void* _virtualAddr;
	void* _src;
	void* _code;

	size_t _origLen;
	size_t _len;

	bool _isBefore;
	bool _SafeVirtual;

	DWORD _origAddr;

	CMemCpy *MemCpy_orig;
	CMemCpy *asmJmpAddr;
	CMemCpy *asmJmpExitAddr;
	CMemSet *asmJmp;
	CMemSet *asmJmpExit;
	CMemSet *MemSet;
	CMemSet *MemSet_orig;

	void ModifyCode()
	{
		if (_origLen < 5)
			return;
		DWORD VirtualCode = (DWORD)_virtualAddr + (!_isBefore ? _len : 0);
		if (*(byte*)VirtualCode != 0xE8 && *(byte*)VirtualCode != 0xE9)
			return;

		DWORD dwRelative = *(DWORD*)(VirtualCode + 1);
		DWORD dwFunc = GetFuncAddress((DWORD)_src, dwRelative);
		dwRelative = GetRelativeAddress(VirtualCode, dwFunc);
		memcpy((void*)(VirtualCode + 1), &dwRelative, 4);
	}
	DWORD GetFuncAddress(DWORD dwFrom /*hook addr*/, DWORD dwRelative /*To addr*/)
	{
		return dwRelative + (dwFrom + 5);
	}
};

// Наследник CVirtualCode, в качестве "вашего" кода передает код перехода в вашу функцию.
class CHookJmp : public CVirtualCode
{
public:
	CHookJmp(void* addrForHook, void(* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize(addrForHook, pFunc);
	}
	CHookJmp(DWORD addrForHook, void(* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize((void*)addrForHook, pFunc);
	}

	// Возвращает адрес для совместимого выхода из хука
	DWORD GetExitAddress()
	{
		return _codeAddr + 5;
	}
private:
	void Initialize(void* addrForHook, void(*pFunc)())
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[5];
		// Записываем asm инструкцию
		code[0] = 0xE9; //jmp

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr, (DWORD)pFunc);
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy((DWORD)code + 1, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, 5, false);
	}
};

// Наследник CVirtualCode, в качестве "вашего" кода передает код вызова в вашу функцию, при этом флаги сохраняются.
class CHookCall : public CVirtualCode
{
public:
	CHookCall(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90", 7, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize(addrForHook, pFunc);
	}
	CHookCall(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90", 7, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[7];
		// Записываем asm инструкции
		code[0] = 0x9C; //pushfd
		code[1] = 0xE8; //call
		code[6] = 0x9D; //popfd

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 1, (DWORD)pFunc);
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy((DWORD)code + 2, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, 7, false);
	}
};
// Наследник CVirtualCode, в качестве "вашего" кода передает код вызова в вашу функцию, при этом сохраняются флаги и регистры.
class CHookCallSafe : public CVirtualCode
{
public:
	CHookCallSafe(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90", 9, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize(addrForHook, pFunc);
	}
	CHookCallSafe(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90", 9, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[9];
		// Записываем asm инструкции
		code[0] = 0x60; //pushad
		code[1] = 0x9C; //pushfd
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd
		code[8] = 0x61; //popad

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 2, (DWORD)pFunc);
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy((DWORD)code + 3, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, 9, false);
	}
};
// Наследник CVirtualCode, в качестве "вашего" кода передает код вызова в вашу функцию, при этом ничего не сохраняется.
class CHookCallNoSafe : public CVirtualCode
{
public:
	CHookCallNoSafe(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize(addrForHook, pFunc);
	}
	CHookCallNoSafe(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!
		if (size < 5)
			return;

		// Создаем хук
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[5];
		// Записываем asm инструкции
		code[1] = 0xE8; //call

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr, (DWORD)pFunc);
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy((DWORD)code + 1, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, 5, false);
	}
};

// Хук функции Present в DX9. Из параметров передает только IDirect3DDevice9*. || TODO: прикрутить прокси D3D9
class CHookD3DPresent : public CVirtualCode
{
public:
	CHookD3DPresent(void(CALLBACK* pFunc)(IDirect3DDevice9*), bool CodeBefore = false)
		: CVirtualCode(17, 5, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90", 8, true)
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!

		// Создаем хук
		Initialize(pFunc);
	}

private:
	void Initialize( void(CALLBACK* pFunc)(IDirect3DDevice9*) )
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[8];
		// Записываем asm инструкции
		code[0] = 0x9C; //pushfd
		code[1] = 0x50; //push eax
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 2, (DWORD)pFunc);
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy((DWORD)code + 3, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy(_codeAddr, code, 8, false);
	}
};

// Хук функции Reset в DX9. Из параметров передает только IDirect3DDevice9*. || TODO: прикрутить прокси D3D9
class CHookD3DReset : public CVirtualCode
{
public:
	CHookD3DReset( void(CALLBACK* pFunc)(IDirect3DDevice9*), bool CodeBefore = false )
		: CVirtualCode( 16, 5, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90", 8, true )
	{
		// Если количество байт меньше 5, то вызвать call и jmp данным способом мы не можем!

		// Создаем хук
		Initialize( pFunc );
	}

private:
	void Initialize( void(CALLBACK* pFunc)(IDirect3DDevice9*) )
	{
		// Удаляем класс, который сейчас выступал заглушкой
		delete MemCpy_code;
		// Создаем массив байт под код перехода
		byte code[8];
		// Записываем asm инструкции
		code[0] = 0x9C; //pushfd
		code[1] = 0x50; //push eax
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd

		// Получаем относительный адрес
		DWORD RelativeAddress = GetRelativeAddress( _codeAddr + 2, (DWORD)pFunc );
		// Записываем его в массив выделенный для кода и сразу удаляем класс, ибо он нам больше не нужен в данном виде
		MemCpy_code = new CMemCpy( (DWORD)code + 3, &RelativeAddress, 4, false );
		delete MemCpy_code;

		// Записываем код в виртуальную память
		MemCpy_code = new CMemCpy( _codeAddr, code, 8, false );
	}
};