#include "main.h"


CBSInfo::CBSInfo( void* BS )
{
	_BS = (BitStream*)BS;
}

void CBSInfo::Write( void* data, size_t size )
{
	_BS->Write( (const char*)data, size );
}
void CBSInfo::WriteComp( byte data )
{
	_BS->WriteCompressed( data );
}
void CBSInfo::WriteComp( WORD data )
{
	_BS->WriteCompressed( data );
}
void CBSInfo::WriteComp( DWORD data )
{
	_BS->WriteCompressed( data );
}
void CBSInfo::WriteBits( unsigned char* input, int numberOfBitsToWrite, bool rightAlignedBits )
{
	_BS->WriteBits( input, numberOfBitsToWrite, rightAlignedBits );
}
void CBSInfo::SetWriteOffset( int offset )
{
	_BS->SetWriteOffset( offset );
}
int CBSInfo::GetWriteOffset()
{
	return _BS->GetWriteOffset();
}

bool CBSInfo::Read( void* data, size_t size )
{
	return _BS->Read( (char*)data, size );
}
bool CBSInfo::ReadComp( byte &data )
{
	return _BS->ReadCompressed( data );
}
bool CBSInfo::ReadComp( WORD &data )
{
	return _BS->ReadCompressed( data );
}
bool CBSInfo::ReadComp( DWORD &data )
{
	return _BS->ReadCompressed( data );
}
bool CBSInfo::ReadBit()
{
	return _BS->ReadBit();
}
void CBSInfo::SetReadOffset( int offset )
{
	_BS->SetReadOffset( offset );
}
int CBSInfo::GetReadOffset()
{
	return _BS->GetReadOffset();
}

void CBSInfo::SetNumberOfBitsAllocated( unsigned int lengthInBits )
{
	_BS->SetNumberOfBitsAllocated( lengthInBits );
}
int CBSInfo::GetNumberOfUnreadBits()
{
	return _BS->GetNumberOfUnreadBits();
}
int CBSInfo::GetNumberOfBitsUsed()
{
	return _BS->GetNumberOfBitsUsed();
}
int CBSInfo::GetNumberOfBytesUsed()
{
	return _BS->GetNumberOfBytesUsed();
}

DLLEXPORTC CBSInfo* CALLBACK CreateBSInfo( void* BS )
{
	return new CBSInfo(BS);
}
DLLEXPORTC void CALLBACK DestoryBSInfo( CBSInfo* &pBS )
{
	delete pBS;
	pBS = nullptr;
}