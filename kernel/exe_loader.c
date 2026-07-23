/* ===================================================================
   BLUE OS - kernel/exe_loader.c
   Carregador NATIVO de Executáveis Windows PE (.exe 32-bit Bare-Metal)
   =================================================================== */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// Assinaturas mágicas do formato PE/DOS
#define DOS_MAGIC_MZ 0x5A4D       // ASCII 'M' 'Z' (0x5A4D)
#define PE_MAGIC_NT  0x00004550   // ASCII 'P' 'E' \0 \0

// 1. Cabeçalho DOS (Primeiros 64 bytes de todo arquivo .exe)
typedef struct {
    uint16_t e_magic;    // Signature 'MZ'
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;   // Offset para o cabeçalho PE (NT Header)
} ImageDosHeader;

// 2. Cabeçalho de Arquivo COFF/PE
typedef struct {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} ImageFileHeader;

// 3. Cabeçalho Opcional (PE32 - 32 bits)
typedef struct {
    uint16_t Magic;
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint; // Offset RVA do ponto de entrada
    uint32_t BaseOfCode;
    uint32_t BaseOfData;
    uint32_t ImageBase;           // Endereço base preferencial na RAM
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
} ImageOptionalHeader32;

// 4. Estrutura Unificada dos Cabeçalhos NT
typedef struct {
    uint32_t Signature;           // 'PE\0\0'
    ImageFileHeader FileHeader;
    ImageOptionalHeader32 OptionalHeader;
} ImageNtHeaders32;

// 5. Cabeçalho de Seção (.text, .data, .rdata, etc)
typedef struct {
    uint8_t  Name[8];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;     // RVA de destino na RAM
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;    // Offset no arquivo físico
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} ImageSectionHeader;

// Valida as assinaturas do DOS e PE do arquivo executável
int verify_pe_headers(const uint8_t* buffer) {
    if (buffer == (void*)0) return 0;

    // 1. Verifica a assinatura DOS 'MZ'
    ImageDosHeader* dos_hdr = (ImageDosHeader*)buffer;
    if (dos_hdr->e_magic != DOS_MAGIC_MZ) {
        return 0; // Não é um arquivo .exe
    }

    // 2. Localiza e verifica a assinatura NT 'PE\0\0'
    ImageNtHeaders32* nt_hdrs = (ImageNtHeaders32*)(buffer + dos_hdr->e_lfanew);
    if (nt_hdrs->Signature != PE_MAGIC_NT) {
        return 0; // Cabeçalho PE inválido
    }

    return 1; // Executável PE válido!
}

// Carrega o binário PE na RAM e executa a instrução inicial
int load_and_run_pe(const uint8_t* exe_data) {
    if (!verify_pe_headers(exe_data)) {
        return -1; // Falha na validação do .exe
    }

    ImageDosHeader* dos_hdr = (ImageDosHeader*)exe_data;
    ImageNtHeaders32* nt_hdrs = (ImageNtHeaders32*)(exe_data + dos_hdr->e_lfanew);

    // Ponteiro para a tabela de seções (localizado imediatamente após o OptionalHeader)
    uint32_t section_offset = dos_hdr->e_lfanew + sizeof(uint32_t) + 
                             sizeof(ImageFileHeader) + 
                             nt_hdrs->FileHeader.SizeOfOptionalHeader;

    ImageSectionHeader* section = (ImageSectionHeader*)(exe_data + section_offset);

    // 1. Mapeia as seções (.text, .data) no espaço de memória virtual/física
    for (uint16_t i = 0; i < nt_hdrs->FileHeader.NumberOfSections; i++) {
        // Endereço de destino na RAM = ImageBase + VirtualAddress
        uint8_t* dest = (uint8_t*)(nt_hdrs->OptionalHeader.ImageBase + section[i].VirtualAddress);
        const uint8_t* src = exe_data + section[i].PointerToRawData;

        // Copia os dados brutos da seção do arquivo para a RAM
        for (uint32_t b = 0; b < section[i].SizeOfRawData; b++) {
            dest[b] = src[b];
        }

        // Preenche o restante do espaço alocado da seção com zero (ex: seções .bss)
        for (uint32_t b = section[i].SizeOfRawData; b < section[i].VirtualSize; b++) {
            dest[b] = 0;
        }
    }

    // 2. Calcula o Ponto de Entrada Real (ImageBase + AddressOfEntryPoint)
    uint32_t entry_address = nt_hdrs->OptionalHeader.ImageBase + 
                             nt_hdrs->OptionalHeader.AddressOfEntryPoint;

    typedef void (*entry_func_t)(void);
    entry_func_t entry_point = (entry_func_t)entry_address;

    // 3. Pulo da CPU para o ponto de execução do .exe
    entry_point();

    return 0;
}
