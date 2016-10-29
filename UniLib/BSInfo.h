#pragma once
class DLLEXPORT CBSInfo
{
public:
	CBSInfo( void* BS );

	virtual void Write( void* data, size_t size );
	virtual void WriteComp( byte data );
	virtual void WriteComp( WORD data );
	virtual void WriteComp( DWORD data );
	virtual void WriteBits( unsigned char* input, int numberOfBitsToWrite, bool rightAlignedBits = true );
	virtual void SetWriteOffset( int offset );
	virtual int GetWriteOffset();

	virtual bool Read( void* data, size_t size );
	virtual bool ReadComp( byte &data );
	virtual bool ReadComp( WORD &data );
	virtual bool ReadComp( DWORD &data );
	virtual bool ReadBit();
	virtual void SetReadOffset( int offset );
	virtual int GetReadOffset();

	virtual void SetNumberOfBitsAllocated( unsigned int lengthInBits );
	virtual int GetNumberOfUnreadBits();
	virtual int GetNumberOfBitsUsed();
	virtual int GetNumberOfBytesUsed();

private:
	BitStream* _BS;
};

DLLEXPORTC CBSInfo* CALLBACK CreateBSInfo( void* BS );
DLLEXPORTC void CALLBACK DestoryBSInfo( CBSInfo* &pBS );