#pragma once
#include "CMem.hpp"

// ��������� ��� ������ ���� �� ���������
struct stFindPatern
{
	DWORD dwAddress;
	DWORD dwLen;
	BYTE *bMask;
	char *szMask;
};

// ���������� ����� ���������� ���� � ����������� ������ � ��������� � ���� ��� ���, � ������ ��� �����.
class CVirtualCode
{
public:
	CVirtualCode(void* src, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		// �������� ��������� ��� �����������
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//�� �� ����� ��������� ������� jmp ������� ��� 0xE9 � ������������ ����� 5 ����
		if (_origLen < 5)
			return;

		// �������� ������
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// ����������� ������ ������������� ����
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// �������� ������ � ��������� ������
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// ���������� ������ ��������� ���������� jmp � call
		ModifyCode();
		// �������� ������������ ���������� ������
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// ����������� ������ ����������������� ���� � ������
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// �������� ���������������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// ���������� � ������������ ��� ���������� jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// �������� ������������� �����, ��� �������� � ����������� ������
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// ���������� ���������� jmp � ����� ����������� ������
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// �������� ������������� �����, ��� ������ �� ����������� ������
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
		asmJmpExitAddr = new CMemCpy((DWORD)_virtualAddr + origLen + len + 1, &RelativeAddress, 4, false);
	}
	CVirtualCode(stFindPatern src_base, void* src, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		src = (void*)((DWORD)src + FindPattern(src_base.dwAddress, src_base.dwLen, src_base.bMask, src_base.szMask));
		delete[] src_base.bMask;
		delete[] src_base.szMask;

		// �������� ��������� ��� �����������
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//�� �� ����� ��������� ������� jmp ������� ��� 0xE9 � ������������ ����� 5 ����
		if (_origLen < 5)
			return;

		// �������� ������
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// ����������� ������ ������������� ����
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// �������� ������ � ��������� ������
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// ���������� ������ ��������� ���������� jmp � call
		ModifyCode();
		// �������� ������������ ���������� ������
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// ����������� ������ ����������������� ���� � ������
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// �������� ���������������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// ���������� � ������������ ��� ���������� jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// �������� ������������� �����, ��� �������� � ����������� ������
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// ���������� ���������� jmp � ����� ����������� ������
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// �������� ������������� �����, ��� ������ �� ����������� ������
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
		asmJmpExitAddr = new CMemCpy((DWORD)_virtualAddr + origLen + len + 1, &RelativeAddress, 4, false);
	}
	CVirtualCode(DWORD TableDx9Id, size_t origLen, bool originalBeforeCode, void* code, size_t len, bool SafeVirtual = false)
	{
		static DWORD* vtbl = 0, table;
		table = FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
		memcpy(&vtbl, (void*)(table + 2), 4);
		void *src = (void*)vtbl[TableDx9Id];

		// �������� ��������� ��� �����������
		_src = src;
		_code = code;
		_origLen = origLen;
		_len = len;
		_isBefore = originalBeforeCode;
		_SafeVirtual = SafeVirtual;
		MemSet_orig = nullptr;

		//�� �� ����� ��������� ������� jmp ������� ��� 0xE9 � ������������ ����� 5 ����
		if (_origLen < 5)
			return;

		// �������� ������
		_virtualAddr = VirtualAlloc(NULL, origLen + len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(_virtualAddr, 0x90, origLen + len + 5);

		// ����������� ������ ������������� ����
		_origAddr = (DWORD)_virtualAddr + (!_isBefore ? len : 0);
		// �������� ������ � ��������� ������
		MemCpy_orig = new CMemCpy(_origAddr, src, origLen, false);
		// ���������� ������ ��������� ���������� jmp � call
		ModifyCode();
		// �������� ������������ ���������� ������
		MemSet = new CMemSet(src, 0x90, origLen, false);

		// ����������� ������ ����������������� ���� � ������
		_codeAddr = (DWORD)_virtualAddr + (_isBefore ? _origLen : 0);
		// �������� ���������������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, len, false);

		// ���������� � ������������ ��� ���������� jmp
		CMemSet *asmJmp = new CMemSet(src, 0xE9, 1, false);

		// �������� ������������� �����, ��� �������� � ����������� ������
		DWORD RelativeAddress = GetRelativeAddress((DWORD)src, (DWORD)_virtualAddr);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
		asmJmpAddr = new CMemCpy((DWORD)src + 1, &RelativeAddress, 4, false);

		// ���������� ���������� jmp � ����� ����������� ������
		asmJmpExit = new CMemSet((DWORD)_virtualAddr + origLen + len, 0xE9, 1, false);

		// �������� ������������� �����, ��� ������ �� ����������� ������
		RelativeAddress = GetRelativeAddress((DWORD)_virtualAddr + origLen + len, (DWORD)src + origLen);
		// ���������� ��������� ������������� ����� � �������� �������� ���������� jmp
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
			return; //������ �������, �� ������ �� ���������

		// ��� ��� ������ �� ����� - ������� ��� �����
		delete MemCpy_code;
		// ��������������� ������������ ����������
		NopOriginalCode(false);

		if ( !_SafeVirtual ) {
			// ������� �� ������ ��� ������, ��� �� �� ������ � ������ �����
			delete MemCpy_orig;
			delete asmJmpAddr;
			delete asmJmp;
			delete asmJmpExitAddr;
			delete asmJmpExit;
			// ������� ���� �� ������������� ����, ��� ����� �������������� ���
			delete MemSet;
			// ����������� ������
			VirtualFree(_virtualAddr, _origLen + _len + 5, MEM_RELEASE);
		} else {
			// �������� ������ ���������������� ���
			memset((void*)_codeAddr, 0x90, _len);

			// ��������� ����������� - ������������� ����
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

// ��������� CVirtualCode, � �������� "������" ���� �������� ��� �������� � ���� �������.
class CHookJmp : public CVirtualCode
{
public:
	CHookJmp(void* addrForHook, void(* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize(addrForHook, pFunc);
	}
	CHookJmp(DWORD addrForHook, void(* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize((void*)addrForHook, pFunc);
	}

	// ���������� ����� ��� ������������ ������ �� ����
	DWORD GetExitAddress()
	{
		return _codeAddr + 5;
	}
private:
	void Initialize(void* addrForHook, void(*pFunc)())
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[5];
		// ���������� asm ����������
		code[0] = 0xE9; //jmp

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr, (DWORD)pFunc);
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy((DWORD)code + 1, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, 5, false);
	}
};

// ��������� CVirtualCode, � �������� "������" ���� �������� ��� ������ � ���� �������, ��� ���� ����� �����������.
class CHookCall : public CVirtualCode
{
public:
	CHookCall(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90", 7, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize(addrForHook, pFunc);
	}
	CHookCall(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90", 7, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[7];
		// ���������� asm ����������
		code[0] = 0x9C; //pushfd
		code[1] = 0xE8; //call
		code[6] = 0x9D; //popfd

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 1, (DWORD)pFunc);
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy((DWORD)code + 2, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, 7, false);
	}
};
// ��������� CVirtualCode, � �������� "������" ���� �������� ��� ������ � ���� �������, ��� ���� ����������� ����� � ��������.
class CHookCallSafe : public CVirtualCode
{
public:
	CHookCallSafe(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90", 9, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize(addrForHook, pFunc);
	}
	CHookCallSafe(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90", 9, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[9];
		// ���������� asm ����������
		code[0] = 0x60; //pushad
		code[1] = 0x9C; //pushfd
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd
		code[8] = 0x61; //popad

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 2, (DWORD)pFunc);
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy((DWORD)code + 3, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, 9, false);
	}
};
// ��������� CVirtualCode, � �������� "������" ���� �������� ��� ������ � ���� �������, ��� ���� ������ �� �����������.
class CHookCallNoSafe : public CVirtualCode
{
public:
	CHookCallNoSafe(void* addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode(addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize(addrForHook, pFunc);
	}
	CHookCallNoSafe(DWORD addrForHook, void(CALLBACK* pFunc)(), size_t size = 5, bool CodeBefore = false)
		: CVirtualCode((void*)addrForHook, size, CodeBefore, (byte*)"\x90\x90\x90\x90\x90", 5, (size < 5 ? false : true))
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!
		if (size < 5)
			return;

		// ������� ���
		Initialize((void*)addrForHook, pFunc);
	}
private:
	void Initialize(void* addrForHook, void(CALLBACK* pFunc)())
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[5];
		// ���������� asm ����������
		code[1] = 0xE8; //call

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr, (DWORD)pFunc);
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy((DWORD)code + 1, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, 5, false);
	}
};

// ��� ������� Present � DX9. �� ���������� �������� ������ IDirect3DDevice9*. || TODO: ���������� ������ D3D9
class CHookD3DPresent : public CVirtualCode
{
public:
	CHookD3DPresent(void(CALLBACK* pFunc)(IDirect3DDevice9*), bool CodeBefore = false)
		: CVirtualCode(17, 5, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90", 8, true)
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!

		// ������� ���
		Initialize(pFunc);
	}

private:
	void Initialize( void(CALLBACK* pFunc)(IDirect3DDevice9*) )
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[8];
		// ���������� asm ����������
		code[0] = 0x9C; //pushfd
		code[1] = 0x50; //push eax
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress(_codeAddr + 2, (DWORD)pFunc);
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy((DWORD)code + 3, &RelativeAddress, 4, false);
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy(_codeAddr, code, 8, false);
	}
};

// ��� ������� Reset � DX9. �� ���������� �������� ������ IDirect3DDevice9*. || TODO: ���������� ������ D3D9
class CHookD3DReset : public CVirtualCode
{
public:
	CHookD3DReset( void(CALLBACK* pFunc)(IDirect3DDevice9*), bool CodeBefore = false )
		: CVirtualCode( 16, 5, CodeBefore, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90", 8, true )
	{
		// ���� ���������� ���� ������ 5, �� ������� call � jmp ������ �������� �� �� �����!

		// ������� ���
		Initialize( pFunc );
	}

private:
	void Initialize( void(CALLBACK* pFunc)(IDirect3DDevice9*) )
	{
		// ������� �����, ������� ������ �������� ���������
		delete MemCpy_code;
		// ������� ������ ���� ��� ��� ��������
		byte code[8];
		// ���������� asm ����������
		code[0] = 0x9C; //pushfd
		code[1] = 0x50; //push eax
		code[2] = 0xE8; //call
		code[7] = 0x9D; //popfd

		// �������� ������������� �����
		DWORD RelativeAddress = GetRelativeAddress( _codeAddr + 2, (DWORD)pFunc );
		// ���������� ��� � ������ ���������� ��� ���� � ����� ������� �����, ��� �� ��� ������ �� ����� � ������ ����
		MemCpy_code = new CMemCpy( (DWORD)code + 3, &RelativeAddress, 4, false );
		delete MemCpy_code;

		// ���������� ��� � ����������� ������
		MemCpy_code = new CMemCpy( _codeAddr, code, 8, false );
	}
};