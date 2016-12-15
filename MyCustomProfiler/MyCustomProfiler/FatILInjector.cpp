#include "FatILInjector.h"



FatILInjector::FatILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize):IILInjector(ptr)
{
	
}


FatILInjector::~FatILInjector()
{
}

size_t FatILInjector::GetHeaderSize() {
	return FAT_HEADER_SIZE;
}

void FatILInjector::WriteHeader(void * bytes)
{
	WORD maxStackSize = (WORD)GetFatInfo()->MaxStack + (sizeof(ILCode) / 2); // rough estimation
	DWORD newSize = GetNewMethodSize();

	memcpy((BYTE*)bytes, this->p_FunctionInvokationInfo->GetMethodBytes(), GetHeaderSize());
	memcpy((BYTE*)bytes + sizeof(WORD), &maxStackSize, sizeof(WORD));
	memcpy((BYTE*)bytes + sizeof(WORD) + sizeof(WORD), &newSize, sizeof(DWORD));
}

COR_ILMETHOD_FAT *FatILInjector::GetFatInfo()
{
	return (COR_ILMETHOD_FAT*)p_FunctionInvokationInfo->GetMethodBytes();
}

BOOL FatILInjector::HasSEH()
{
	return (GetFatInfo()->GetFlags() & CorILMethod_MoreSects);
}

ULONG FatILInjector::GetExtraSectionsSize()
{
	return p_FunctionInvokationInfo->GetMethodSize() - GetOldMethodSizeWithoutExtraSections();
}

ULONG FatILInjector::GetOldMethodSizeWithoutExtraSections()
{
	return GetHeaderSize() + GetOldMethodBodySize();
}

ULONG FatILInjector::GetDWORDAlignmentOffset()
{
	if (!HasSEH())
	{
		return 0;
	}
	else
	{
		ULONG totalDelta = ((sizeof(ILCode) + GetOldMethodSizeWithoutExtraSections()) % sizeof(DWORD));

		ULONG newDelta = 0;
		if (totalDelta != 0)
		{
			newDelta = sizeof(DWORD) - totalDelta;
		}

		ULONG oldDelta = (int)((BYTE*)GetFatInfo()->GetSect() - (BYTE*)((BYTE*)p_FunctionInvokationInfo->GetMethodBytes() + GetOldMethodSizeWithoutExtraSections()));

		return newDelta - oldDelta;
	}
}

BOOL FatILInjector::CanRewrite()
{
	if (HasSEH())
	{
		COR_ILMETHOD_DECODER method((const COR_ILMETHOD*)p_FunctionInvokationInfo->GetMethodBytes());
		COR_ILMETHOD_SECT_EH* currentEHSection = (COR_ILMETHOD_SECT_EH*)method.EH;

		do
		{
			for (UINT i = 0; i < currentEHSection->EHCount(); ++i)
			{
				if (!currentEHSection->IsFat() && ((currentEHSection->Small.Clauses[i].TryOffset + sizeof(ILCode)) > 0xFFFF || (currentEHSection->Small.Clauses[i].HandlerOffset + sizeof(ILCode)) > 0xFFFF))
				{
					//Diagnostics::GetInstance()->PrintMessage(" -> Don't log. Can't modify small EH clasue if it's bigger then 2 bytes");
					return FALSE;
				}
			}

			do
			{
				currentEHSection = (COR_ILMETHOD_SECT_EH*)currentEHSection->Next();
			} while (currentEHSection && (currentEHSection->Kind() & CorILMethod_Sect_KindMask) != CorILMethod_Sect_EHTable);

		} while (currentEHSection);
	}

	return TRUE;
}

void FatILInjector::FixSEHSections(LPCBYTE methodBytes, ULONG newILSize)
{
	COR_ILMETHOD_DECODER method((const COR_ILMETHOD*)methodBytes);
	COR_ILMETHOD_SECT_EH* currentEHSection = (COR_ILMETHOD_SECT_EH*)method.EH;

	do
	{
		for (UINT i = 0; i < currentEHSection->EHCount(); ++i)
		{
			if (currentEHSection->IsFat())
			{
				if (currentEHSection->Fat.Clauses[i].Flags == COR_ILEXCEPTION_CLAUSE_FILTER)
				{
					currentEHSection->Fat.Clauses[i].FilterOffset += newILSize;
				}

				currentEHSection->Fat.Clauses[i].TryOffset += newILSize;
				currentEHSection->Fat.Clauses[i].HandlerOffset += newILSize;
			}
			else
			{
				if (currentEHSection->Small.Clauses[i].Flags == COR_ILEXCEPTION_CLAUSE_FILTER)
				{
					currentEHSection->Small.Clauses[i].FilterOffset += newILSize;
				}

				currentEHSection->Small.Clauses[i].TryOffset += newILSize;
				currentEHSection->Small.Clauses[i].HandlerOffset += newILSize;
			}
		}

		do
		{
			currentEHSection = (COR_ILMETHOD_SECT_EH*)currentEHSection->Next();
		} while (currentEHSection && (currentEHSection->Kind() & CorILMethod_Sect_KindMask) != CorILMethod_Sect_EHTable);

	} while (currentEHSection);
}