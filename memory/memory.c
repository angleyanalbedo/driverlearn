#define NEW_STR RTL_CONSTANT_STRING
#include <ntddk.h>
#include <basetsd.h>
#pragma warning(disable : 4100 4201)


typedef struct _DRIVER_OBJECT* PDRIVER_OBJECT;
typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;               //����洢,ָ����һ��LDR_DATA_TABLE_ENTRY�ṹ
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;                             //��ַ
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;                //���������ģ����
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union {
        struct {
            ULONG TimeDateStamp;
        };
        struct {
            PVOID LoadedImports;
        };
    };
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;
//���溯�������ڻ�ȡsys�Ļ���ַ
// һ���������ܴ����������õ�
VOID GetModuleBaseByName(PDRIVER_OBJECT pDriverObj, UNICODE_STRING ModuleName,PVOID* BasePtr,PULONG ImageSize)
{
    PLDR_DATA_TABLE_ENTRY pLdr = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PLIST_ENTRY pCurrentListEntry = NULL;

    PLDR_DATA_TABLE_ENTRY pCurrentModule = NULL;
    pLdr = (PLDR_DATA_TABLE_ENTRY)pDriverObj->DriverSection;
    pListEntry = pLdr->InLoadOrderLinks.Flink;
    pCurrentListEntry = pListEntry->Flink;

    while (pCurrentListEntry != pListEntry) //ǰ�����
    {
        //��ȡLDR_DATA_TABLE_ENTRY�ṹ
        pCurrentModule = CONTAINING_RECORD(pCurrentListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (pCurrentModule->BaseDllName.Buffer != 0)
        {

            if (RtlCompareUnicodeString(&pCurrentModule->BaseDllName, &ModuleName, TRUE) == 0)
            {
                DbgPrint("ModuleName = %wZ ModuleBase = %p ModuleEndBase = %p\r\n",
                    pCurrentModule->BaseDllName,
                    pCurrentModule->DllBase,
                    (LONGLONG)pCurrentModule->DllBase + pCurrentModule->SizeOfImage);
                *BasePtr = pCurrentModule->DllBase;
                *ImageSize = pCurrentModule->SizeOfImage;
            }

        }
        pCurrentListEntry = pCurrentListEntry->Flink;
    }
}

VOID Unload(IN PDRIVER_OBJECT DriverObject) {
	DbgPrint("ubload\r\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegisteryPath) {

	DriverObject->DriverUnload = Unload;
    DbgPrint("diver is load \r\n");

	
    PVOID BasePtr = 0;
    ULONG Size = 0;
    
    UNICODE_STRING SbieDrv = RTL_CONSTANT_STRING(L"SbieDrv.sys");
    
    GetModuleBaseByName(DriverObject, SbieDrv, &BasePtr, &Size);
    DbgPrint("base ptrr %p size %ld", BasePtr, Size);
    if (BasePtr) {
        //DbgPrint("cer is %c", *((PCHAR)BasePtr + 0x34088));
        __try {
            *((PCHAR)BasePtr + 0x34088) = 1;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            DbgPrint("Have Wrong in Write");
        };
        
        
    }
	return STATUS_SUCCESS;

}