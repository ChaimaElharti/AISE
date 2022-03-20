#include "../include/ANSI-color-codes.h"
#include "../include/dbg.h"

#define FTRACE_ENV "FTRACE_ARCH"
/**
 * @brief Get the status of the process for which PID is given.
 *
 * @param pid
 */
void get_status(int pid)
{
    char buf[1024], path[512], *p;
    int fd;
    snprintf(path, 512, "/proc/%d/status", pid);

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        perror("open");
        exit(-1);
    }
    int nb_read;
    while ((nb_read = read(fd, buf, 1024)) > 0)
    {
        if (write(STDOUT_FILENO, buf, nb_read) < 0)
        {
            perror("write\n");
            exit(-1);
        }
    }
}

/**
 * @brief Get the memory maps of the process for which PID is given.
 *
 * @param pid
 */
void get_memory_maps(int pid)
{
    char buf[1024], path[512], *p;
    int fd;
    snprintf(path, 512, "/proc/%d/maps", pid);

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        perror("open");
        exit(-1);
    }
    int nb_read;
    while ((nb_read = read(fd, buf, 1024)) > 0)
    {
        if (write(STDOUT_FILENO, buf, nb_read) < 0)
        {
            perror("write\n");
            exit(-1);
        }
    }
}

/**
 * @brief Duplication d'une chaine de caratère donnée
 *
 * @param original Pointeur sur la chaine de caratère à dupliquer
 * @return char*  Pointeur sur la chaine de caractère dupliquée.
 */
char *strduplication(const char *original)
{
    char *p = strdup(original); //
    if (p == NULL)
    {
        perror("strduplication");
        exit(-1);
    }
    return p;
}

/**
 * @brief Gestionnaire des signaux
 *
 * @param signum Numéro signal
 * @param si Structure siginfo_t
 * @param context
 */
 void handler(int signum, siginfo_t *si, void *context)
{
    switch (signum)
    {
    case SIGABRT:
        printf(B_WHITE "Terminaison anormale, normalement initiée par la fonction abort.\n" reset);

        break;
    case SIGFPE:
        printf(B_WHITE "Opération arithmétique erronée, telle que division du zéro ou opération entraînant un dépassement de capacité. Ou manipulation de valeur numérique sans virgule flottante.\n" reset);
        switch (si->si_code)
        {
        case FPE_FLTDIV:
            printf("Floating-point divided-by-zero at %p\n", si->si_addr);
            break;
        case FPE_FLTINV:
            printf("Invalide floating-point operation at %p\n", si->si_addr);
            break;
        case FPE_FLTOVF:
            printf("Floating-point overflow at %p\n", si->si_addr);
            break;
        case FPE_FLTRES:
            printf("Floating-point inexact result at %p\n", si->si_addr);
            break;
        case FPE_FLTUND:
            printf("Floating-point underflow at %p\n", si->si_addr);
            break;
        case FPE_INTDIV:
            printf("Integer divided-by-zero at %p\n", si->si_addr);
            break;
        case FPE_INTOVF:
            printf("Integer overflow at %p\n", si->si_addr);
            break;
        case FPE_FLTSUB:
            printf("Subscript out of range at %p\n", si->si_addr);
            break;
        }
        break;
    case SIGILL:
        printf(B_WHITE "Exécution d'une instruction non conforme (corruption du code ou à une tentative d'exécution de données).\n" reset);
        switch (si->si_code)
        {
        case ILL_BADSTK:
            printf("Internal stack error at %p\n", si->si_addr);
            break;
        case ILL_COPROC:
            printf("Coprocessor error at %p\n", si->si_addr);
            break;
        case ILL_ILLADR:
            printf("Illegal addressing mode at %p\n", si->si_addr);
            break;
        case ILL_ILLOPC:
            printf("Illegal opcode at %p\n", si->si_addr);
            break;
        case ILL_ILLOPN:
            printf("Illegal operand at %p\n", si->si_addr);
            break;
        case ILL_ILLTRP:
            printf("Illegal trap at %p\n", si->si_addr);
            break;
        case ILL_PRVOPC:
            printf("Privileged opcode at %p\n", si->si_addr);
            break;
        case ILL_PRVREG:
            printf("Priviledged register at %p\n", si->si_addr);
            break;
        }
        break;
    case SIGINT:
        printf(B_WHITE "Signal d'interruption du programme (peut-etre par l'utilisateur)\n" reset);
        break;
    case SIGSEGV:
        printf(B_WHITE "Acces a un segment de memoire virtuelle non mappe en memoire physique ou tentative de modification d'un segment de memoire configure en lecture seule.\n" reset);
        switch (si->si_code)
        {
        case SEGV_ACCERR:
            printf("Invalid permission for mapped object at %p\n", si->si_addr);
            break;
        case SEGV_MAPERR:
            printf("Address not mapped to object at %p\n", si->si_addr);
            break;
        case SEGV_BNDERR:
            printf("Failed address bound checks at %p\n", si->si_addr);
            break;
        case SEGV_PKUERR:
            printf("Access was denied by memory protected by keys at %p\n", si->si_addr);
            break;
        }
        break;
    case SIGCHLD:

        printf(B_WHITE "\n" reset);
        switch (si->si_code)
        {
        case CLD_CONTINUED:
            printf("Child continued by SIGCONT\n");
            break;
        case CLD_DUMPED:
            printf("Child terminated abnormally, with core dump\n");
            break;
        case CLD_EXITED:
            printf("Child exited\n");
            break;
        case CLD_KILLED:
            printf("Child terminated abnormally, without core dump\n");
            break;
        case CLD_STOPPED:
            printf("Child stopped\n");
            break;
        case CLD_TRAPPED:
            printf("Traced child has stopped\n");
            break;
        }
        break;
    case SIGBUS:
        printf(B_WHITE "Erreur Hardware\n" reset);
        switch (si->si_code)
        {
        case BUS_ADRALN:
            printf("Invalid address alignment at %p\n", si->si_addr);
            break;
        case BUS_ADRERR:
            printf("Nonexistence physical address at %p\n", si->si_addr);
            break;
        case BUS_MCEERR_AO:
            printf("Hardware memory error; action optionalat %p\n", si->si_addr);
            break;
        case BUS_MCEERR_AR:
            printf("Hardware memory error; action required at %p\n", si->si_addr);
            break;
        case BUS_OBJERR:
            printf("Object-specific hardware error.at %p\n", si->si_addr);
            break;
        }
        break;
        // case SIGTRAP:
        //     printf(B_WHITE "RECEIVE SIGTRAP\n");
        //     switch (si->si_code)
        //     {
        //     case TRAP_BRANCH:
        //         printf("Process branch trap \n");
        //         break;
        //     case TRAP_BRKPT:
        //         printf("Process breakpoint\n");
        //         break;
        //     case TRAP_HWBKPT:
        //         printf("Hadware breakpoint/watchpoint");
        //     case TRAP_TRACE:
        //         printf("Process trace trap\n");
        //     }
        //     break;

    default:
        printf("Receive signal %d (%s)\n", signum, strsignal(signum));
    }
}

/**
 * @brief Get the Symbol object
 *
 * @param h
 * @return int
 */
int getSymbol(struct handle *h)
{
    int i, j, k;
    char *symStrTable;
    Elf32_Ehdr *ehdr32;
    Elf32_Shdr *shdr32; /* section header */
    Elf32_Sym *symtab32;

    Elf64_Ehdr *ehdr64;
    Elf64_Shdr *shdr64; /* section header */
    Elf64_Sym *symtab64;

    int st_type;
    int st_bind;

    h->loccount = 0;
    h->dyncount = 0;
    /*  Gerer les differentes architectures */

    switch (options.arch)
    {
    case 32:
        ehdr32 = h->elf32->ehdr; /* ELF header */
        shdr32 = h->elf32->shdr; /* section header */
                                 /*
                         
                                      *Sachant qu'on a e_shnum sections,
                                      *parcourir chaque section puis identifier la section "(SHT_SYMTAB)" ou "SHT_SYNSYM"
                                      * recuperer les informations sur les symboles.
                                     */
        for (i = 0; i < ehdr32->e_shnum; i++)
        {
            if (shdr32[i].sh_type == SHT_SYMTAB || shdr32[i].sh_type == SHT_DYNSYM)
            {
                symStrTable = (char *)&h->map[shdr32[shdr32[i].sh_link].sh_offset];
                symtab32 = (Elf32_Sym *)&h->map[shdr32[i].sh_offset];

                for (j = 0; j < shdr32[i].sh_size / sizeof(Elf32_Sym); j++, symtab32++)
                {

                    st_type = ELF32_ST_TYPE(symtab32->st_info); /* type du symbole : FUNC , OBJECT, etc..*/
                    st_bind = ELF32_ST_BIND(symtab32->st_info); /* binding : WEAK, LOCAL, GLOBAL , etc..*/

                    if (st_type != STT_FUNC)
                        continue;

                    switch (shdr32[i].sh_type)
                    {
                    case SHT_SYMTAB:
                        h->locsyms[h->loccount].name = strduplication(&symStrTable[symtab32->st_name]);
                        h->locsyms[h->loccount].value = symtab32->st_value;
                        h->locsyms[h->loccount].bind = st_bind;
                        h->locsyms[h->loccount].type = st_type;
                        h->loccount++;
                        break;
                    case SHT_DYNSYM:
                        h->dynsyms[h->dyncount].name = strduplication(&symStrTable[symtab32->st_name]);
                        h->dynsyms[h->dyncount].value = symtab32->st_value;
                        h->dynsyms[h->dyncount].bind = st_bind;
                        h->dynsyms[h->dyncount].type = st_type;
                        h->dyncount++;
                        break;
                    }
                }
            }
        }
        h->elf32->stringTable = (char *)&h->map[shdr32[ehdr32->e_shstrndx].sh_offset];
        for (i = 0; i < ehdr32->e_shnum; i++)
        {
            if (!strcmp(&h->elf32->stringTable[shdr32[i].sh_name], ".plt"))
            {
                for (k = 0, j = 0; j < shdr32[i].sh_size; j += 16)
                {
                    if (j >= 16)
                    {
                        h->dynsyms[k++].value = shdr32[i].sh_addr + j;
                    }
                }
                break;
            }
        }
        break;
    case 64:
        ehdr64 = h->elf64->ehdr;
        shdr64 = h->elf64->shdr;

        for (i = 0; i < ehdr64->e_shnum; i++)
        {
            if (shdr64[i].sh_type == SHT_SYMTAB || shdr64[i].sh_type == SHT_DYNSYM)
            {
                symStrTable = (char *)&h->map[shdr64[shdr64[i].sh_link].sh_offset];
                symtab64 = (Elf64_Sym *)&h->map[shdr64[i].sh_offset];

                for (j = 0; j < shdr64[i].sh_size / sizeof(Elf64_Sym); j++, symtab64++)
                {

                    st_type = ELF64_ST_TYPE(symtab64->st_info);
                    st_bind = ELF64_ST_BIND(symtab64->st_info);

                    if (st_type != STT_FUNC)
                        continue;
                    switch (shdr64[i].sh_type)
                    {
                    case SHT_SYMTAB:
                        h->locsyms[h->loccount].name = strduplication(&symStrTable[symtab64->st_name]);
                        h->locsyms[h->loccount].value = symtab64->st_value;
                        h->locsyms[h->loccount].bind = st_bind;
                        h->locsyms[h->loccount].type = st_type;
                        h->loccount++;
                        break;
                    case SHT_DYNSYM:
                        h->dynsyms[h->dyncount].name = strduplication(&symStrTable[symtab64->st_name]);
                        h->dynsyms[h->dyncount].value = symtab64->st_value;
                        h->dynsyms[h->dyncount].bind = st_bind;
                        h->dynsyms[h->dyncount].type = st_type;
                        h->dyncount++;
                        break;
                    }
                }
            }
        }
        h->elf64->stringTable = (char *)&h->map[shdr64[ehdr64->e_shstrndx].sh_offset];
        for (i = 0; i < ehdr64->e_shnum; i++)
        {
            if (!strcmp(&h->elf64->stringTable[shdr64[i].sh_name], ".plt"))
            {
                for (k = 0, j = 0; j < shdr64[i].sh_size; j += 16)
                {
                    if (j >= 16)
                    {
                        h->dynsyms[k++].value = shdr64[i].sh_addr + j;
                    }
                }
                break;
            }
        }
        return 0;
    }
}

/* Localiser le segment dynamique propre aux fichiers faisant appel a des libraries
 * dont la resolution de liens se fait dynamiquement
 */
void locate_dynamic_segment(struct handle *h)
{
    int i;
    switch (options.arch)
    {
    case 32:
        h->elf32->dyn = NULL;
        for (i = 0; i < h->elf32->ehdr->e_phnum; i++)
        {
            if (h->elf32->phdr[i].p_type == PT_DYNAMIC)
            {
                h->elf32->dyn = (Elf32_Dyn *)&h->map[h->elf32->phdr[i].p_offset];
                break;
            }
        }
        break;
    case 64:
        h->elf64->dyn = NULL;
        for (i = 0; i < h->elf64->ehdr->e_phnum; i++)
        {
            if (h->elf64->phdr[i].p_type == PT_DYNAMIC)
            {
                h->elf64->dyn = (Elf64_Dyn *)&h->map[h->elf64->phdr[i].p_offset];
                break;
            }
        }
        break;
    }
}

/* Allocation memoire sur le tas selon le besoin*/
void *memAlloc(unsigned int len)
{
    uint8_t *mem = malloc(len);
    if (!mem)
    {
        perror("malloc");
        exit(-1);
    }
    return mem;
}

/**
 * @brief Charger le elf en memoire
 *
 * @param h
 */
void load_elf_section(struct handle *h)
{
    Elf32_Ehdr *ehdr32;
    Elf32_Shdr *shdr32;
    Elf64_Ehdr *ehdr64;
    Elf64_Shdr *shdr64;

    char *StringTable;
    int i;

    h->shdr_count = 0;
    switch (options.arch)
    {
    case 32:
        StringTable = h->elf32->stringTable;
        ehdr32 = h->elf32->ehdr;
        shdr32 = h->elf32->shdr;

        for (i = 0; i < ehdr32->e_shnum; i++)
        {
            h->sh_range[i].sh_name = strduplication(&StringTable[shdr32[i].sh_name]);
            h->sh_range[i].sh_addr = shdr32[i].sh_addr;
            h->sh_range[i].sh_size = shdr32[i].sh_size;
            if (h->shdr_count == NB_MAX_SECTIONS)
                break;
            h->shdr_count++;
        }
        break;
    case 64:
        StringTable = h->elf64->stringTable;
        ehdr64 = h->elf64->ehdr;
        shdr64 = h->elf64->shdr;

        for (i = 0; i < ehdr64->e_shnum; i++)
        {
            h->sh_range[i].sh_name = strduplication(&StringTable[shdr64[i].sh_name]);
            h->sh_range[i].sh_addr = shdr64[i].sh_addr;
            h->sh_range[i].sh_size = shdr64[i].sh_size;
            if (h->shdr_count == NB_MAX_SECTIONS)
                break;
            h->shdr_count++;
        }
        break;
    }
}

/**
 * @brief Projection en memoire du elf 32bit
 *
 * @param h
 */
void MapElf32(struct handle *h)
{
    int fd;
    struct stat st;

    if ((fd = open(h->path, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Unable to open %s: %s\n", h->path, strerror(errno));
        exit(-1);
    }

    if (fstat(fd, &st) < 0)
    {
        perror("fstat");
        exit(-1);
    }

    h->map = (uint8_t *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (h->map == MAP_FAILED)
    {
        perror("mmap");
        exit(-1);
    }

    h->elf32->ehdr = (Elf32_Ehdr *)h->map;
    h->elf32->shdr = (Elf32_Shdr *)(h->map + h->elf32->ehdr->e_shoff);
    h->elf32->phdr = (Elf32_Phdr *)(h->map + h->elf32->ehdr->e_phoff);

    h->elf32->stringTable = (char *)&h->map[h->elf32->shdr[h->elf32->ehdr->e_shstrndx].sh_offset];

    if (h->elf32->ehdr->e_shnum > 0 && h->elf32->ehdr->e_shstrndx != SHN_UNDEF)
        load_elf_section(h);
}

/**
 * @brief Projection en memoire du elf 64bit
 *
 * @param h
 */
void MapElf64(struct handle *h)
{
    int fd;
    struct stat st;

    if ((fd = open(h->path, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Unable to open %s: %s\n", h->path, strerror(errno));
        exit(-1);
    }

    if (fstat(fd, &st) < 0)
    {
        perror("fstat");
        exit(-1);
    }

    h->map = (uint8_t *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (h->map == MAP_FAILED)
    {
        perror("mmap");
        exit(-1);
    }

    h->elf64->ehdr = (Elf64_Ehdr *)h->map;
    h->elf64->shdr = (Elf64_Shdr *)(h->map + h->elf64->ehdr->e_shoff);
    h->elf64->phdr = (Elf64_Phdr *)(h->map + h->elf64->ehdr->e_phoff);

    h->elf64->stringTable = (char *)&h->map[h->elf64->shdr[h->elf64->ehdr->e_shstrndx].sh_offset];

    if (h->elf64->ehdr->e_shnum > 0 && h->elf64->ehdr->e_shstrndx != SHN_UNDEF)
        load_elf_section(h);
}

/**
 * @brief Avoir l'offset d'une section donnee
 *
 * @param h
 * @param section_name Nom de la section dont on cherche l'adresse
 * @return uint8_t*
 */
uint8_t *get_section_data(struct handle *h, const char *section_name)
{

    char *StringTable;
    int i;

    switch (options.arch)
    {
    case 32:
        StringTable = h->elf32->stringTable;
        for (i = 0; i < h->elf32->ehdr->e_shnum; i++)
        {
            if (!strcmp(&StringTable[h->elf32->shdr[i].sh_name], section_name))
            {
                return &h->map[h->elf32->shdr[i].sh_offset];
            }
        }
        break;
    case 64:
        StringTable = h->elf64->stringTable;
        for (i = 0; i < h->elf64->ehdr->e_shnum; i++)
        {
            if (!strcmp(&StringTable[h->elf64->shdr[i].sh_name], section_name))
            {
                return &h->map[h->elf64->shdr[i].sh_offset];
            }
        }
        break;
    }

    return NULL;
}

/**
 * @brief Permet d'identifier une librairie dynamique donnee
 *
 * @param h
 * @param xset  offset
 * @return char* pointeur sur le nom de la lib chargee dynamiquement
 */
char *get_dt_strtab_name(struct handle *h, int xset)
{
    static char *dyn_strtbl;

    if (!dyn_strtbl && !(dyn_strtbl = get_section_data(h, ".dynstr")))
        printf("[!] Could not locate .dynstr section\n");

    return dyn_strtbl + xset;
}

/**
 * @brief Recuperation des noms de chaque librarie et stockage
 * dans le tableau libnames;
 *
 * @param h
 */
void parse_dynamic_dt_needed(struct handle *h)
{
    char *symstr;
    int i, nb_entries;
    Elf32_Dyn *dyn32;
    Elf64_Dyn *dyn64;

    locate_dynamic_segment(h);
    h->libcount = 0;

    switch (options.arch)
    {
    case 32:
        dyn32 = h->elf32->dyn;
        for (i = 0; dyn32[i].d_tag != DT_NULL; i++)
        {
            if (dyn32[i].d_tag == DT_NEEDED)
            {
                symstr = get_dt_strtab_name(h, dyn32[i].d_un.d_val);
                h->libnames[h->libcount++] = (char *)strduplication(symstr);
            }
        }
        break;
    case 64:
        dyn64 = h->elf64->dyn;
        for (i = 0; dyn64[i].d_tag != DT_NULL; i++)
        {
            if (dyn64[i].d_tag == DT_NEEDED)
            {
                symstr = get_dt_strtab_name(h, dyn64[i].d_un.d_val);
                h->libnames[h->libcount++] = (char *)strduplication(symstr);
            }

        }
        break;
    }
}

/**
 * @brief Analyse du processus [a debugger]
 *
 * @param h
 */
void processes_analyze(struct handle *h)
{
    int i;
    struct user_regs_struct reg;
    struct user reg32;
    {
        /* data */
    };

    struct address_space *addrspace = (struct address_space *)memAlloc(sizeof(struct address_space) * 256);

    /*Allocation memoire pour la structure ELF selon
    que l'on soit sur une architecture 32 ou 64,
    mapper ensuite l'executable a cette addresse.
    */
    switch (options.arch)
    {
    case 32:
        h->elf32 = memAlloc(sizeof(struct elf32));
        h->elf64 = NULL;
        MapElf32(h);
        break;
    case 64:
        h->elf64 = memAlloc(sizeof(struct elf64));
        h->elf32 = NULL;
        MapElf64(h);
        break;
    }

    // Récupération des symboles
    getSymbol(h);
    printf(B_BLUE "=================================================================\n" reset);
    printf(B_WHITE "                       Status du programme \n" reset);
    printf(B_BLUE "=================================================================\n" reset);
    printf(reset);
    get_status(h->pid);
    printf("\n");

    printf(B_BLUE "=================================================================\n" reset);
    printf(B_WHITE "                       Maps du programme \n" reset);
    printf(B_BLUE "=================================================================\n" reset);
    get_memory_maps(h->pid);
    printf("\n");
    if (options.elfinfo)
    {
        printf(B_BLUE "=================================================================\n" reset);
        printf(B_WHITE "                   Liste des symboles : \n" reset);
        printf(B_BLUE "=================================================================\n" reset);
        printf(B_PURPLE "Adresses mémoire                             Nom des symboles  \n" reset);
        printf(B_BLUE "=================================================================\n" reset);
        for (i = 0; i < h->loccount; i++)
        {
            if (h->locsyms[i].name == NULL)
            {
                // printf(B_YELLOW);
                printf(B_YELLOW "UNKNOMN: 0x%lx\n" reset, h->locsyms[i].value);
                // printf(reset);
            }
            else
            {
                printf(B_GREEN " 0x%4lx " reset "%50s\n", h->locsyms[i].value, h->locsyms[i].name);
                // printf("%s %15c " B_GREEN "0x%lx\n" reset, h->locsyms[i].name, ' ', h->locsyms[i].value);
                // printf("%s", h->locsyms[i].name);

                // printf(B_GREEN "%20c0x%lx\n" reset, ' ', h->locsyms[i].value);
                // printf(reset);

                // printf("%s 0x%lx\n", h->locsyms[i].name, h->locsyms[i].value);
            }
        }

        for (i = 0; i < h->dyncount; i++)
        {
            if (h->dynsyms[i].name == NULL)
            {
                // printf(B_YELLOW);
                printf(B_YELLOW "UNKNOMN: 0x%lx\n" reset, h->dynsyms[i].value);
                // printf(reset);
            }
            else
            {
                printf(B_GREEN " 0x%4lx " reset "%50s\n", h->dynsyms[i].value, h->dynsyms[i].name);
                // printf("%s", h->dynsyms[i].name);
                // printf(B_GREEN "            0x%lx\n" reset, h->dynsyms[i].value);
                // printf(reset);
            }
        }
        // printf(B_BLUE);
        printf(B_BLUE "=================================================================\n" reset);
        printf(B_WHITE "           Liste des bibliothèques dynamiques : \n" reset);
        printf(B_BLUE "=================================================================\n" reset);

        parse_dynamic_dt_needed(h);

        for (i = 0; i < h->libcount; i++)
        {
            printf("[%d]\t%s\n", i + 1, h->libnames[i]);
        }

        printf(B_BLUE "=================================================================\n" reset);
        printf(B_WHITE "                     REGISTRES : \n" reset);
        printf(B_BLUE "=================================================================\n" reset);
        ptrace(PTRACE_GETREGS, h->pid, NULL, &reg);
        /* switch (options.arch)
         {
         case 32:
             printf(B_Cyan "eax: %llx   " reset, reg32.eax);
             printf(B_Cyan "ebx: %llx   " reset, reg32.ebx);
             printf(B_Cyan "ecx: %llx   " reset, reg32.ecx);
             printf(B_Cyan "edx: %llx\n" reset, reg32.edx);
             printf(B_Cyan "edi: %llx   " reset, reg32.edi);
             printf(B_Cyan "esi: %llx   " reset, reg32.esi);
             printf(B_Cyan "edi: %llx   " reset, reg32.edi);
             printf(B_Cyan "e8:  %llx\n" reset, reg32.e8);
             printf(B_Cyan "e9:  %llx   " reset, reg32.e9);
             printf(B_Cyan "e10: %llx   " reset, reg32.e10);
             printf(B_Cyan "e11: %llx   " reset, reg32e11);
             printf(B_Cyan "e12: %llx\n" reset, reg32.e12);
             printf(B_Cyan "e13: %llx   " reset, reg32.e13);
             printf(B_Cyan "e14: %llx   " reset, reg32.e14);
             printf(B_Cyan "e15: %llx   " reset, reg32.e15);
             printf(B_Cyan "esp: %llx\n" reset, reg32.esp);
             printf(B_Cyan "ebp: %llx" reset, reg32.ebp);
             printf(B_Cyan "eip: %llx\n" reset, reg32.eip);

         case 64: */
        printf(B_Cyan "rax:" reset B_YELLOW " %llx " reset, reg.rax);
        printf(B_Cyan "rbx:" reset B_YELLOW " %llx " reset, reg.rbx);
        printf(B_Cyan "rcx:" reset B_YELLOW " %llx " reset, reg.rcx);
        printf(B_Cyan "rdx:" reset B_YELLOW " %llx\n" reset, reg.rdx);
        printf(B_Cyan "rdi:" reset B_YELLOW " %llx " reset, reg.rdi);
        printf(B_Cyan "rsi:" reset B_YELLOW " %llx " reset, reg.rsi);
        printf(B_Cyan "r14:" reset B_YELLOW " %llx " reset, reg.r14);
        printf(B_Cyan "r8:" reset B_YELLOW " %llx\n" reset, reg.r8);
        printf(B_Cyan "r9:" reset B_YELLOW " %llx " reset, reg.r9);
        printf(B_Cyan "r10:" reset B_YELLOW " %llx " reset, reg.r10);
        printf(B_Cyan "r11:" reset B_YELLOW " %llx\n" reset, reg.r11);
        printf(B_Cyan "r13:" reset B_YELLOW " %llx " reset, reg.r13);
        printf(B_Cyan "r15:" reset B_YELLOW " %llx " reset, reg.r15);
        printf(B_Cyan "rsp:" reset B_YELLOW " %llx\n" reset, reg.rsp);
        printf(B_Cyan "rip:" reset B_YELLOW " %llx\t" reset, reg.rip);
        printf(B_Cyan "rbp:" reset B_YELLOW " %llx\n" reset, reg.rbp);
        printf("\n\n");
    }
}


int main(int argc, char *argv[], char *envp[])
{

    int i, opt, status, skip_getopt = 0, signum;
    pid_t pid;
    char **p, *arch;

    struct handle handle;
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    


    //Partie pour afficher l'adresse du crash

    void *             array[50];
    void *             caller_address;
    char **            messages;
    int                size;
    void * ucontext;
    ucontext_t *   uc;

    uc = (ucontext_t *)ucontext;

    /* Get the address at the time the signal was raised */
    #if defined(__i386__) // gcc specific
        caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
    #endif

    size = backtrace(array, 50);

    /* overwrite sigaction with caller's address */
    array[1] = caller_address;

    messages = backtrace_symbols(array, size);

    /* skip first stack frame (points here) */
    for (i = 2; i < size && messages != NULL; ++i)
    {
        printf(B_BLUE "Adresse du crash : \n");
        fprintf(stderr, "[bt]: (%d) %s \n", i, messages[i]);
    }

    free(messages);


    if (sigaction(SIGSEGV, &sa, (struct sigaction *)NULL) != 0)
    {
        fprintf(stderr, "Error setting signal handler for %d (%s)\n", SIGSEGV, strsignal(SIGSEGV));
        
    }


    if (argc < 2)
    {

    Menu:
        printf(B_Cyan);
        printf("Menu: %s [options entre:-e]<prog>\n", argv[0]);
        printf("[-h] Help\n");
        printf(reset);
        exit(0);
    }
    if (argc == 2 && argv[1][0] == '-')
        goto Menu;

    // Remplir la zone mémoire occupée par opts par des 0

    memset(&options, 0, sizeof(options));
    sigemptyset(&sa.sa_mask);

    // Initialisation de l'architecture par défaut à 64
    options.arch = 64;
    arch = getenv(FTRACE_ENV);
    if (arch != NULL)
    {
        switch (atoi(arch))
        {
        case 32:
            options.arch = 32;
            break;
        case 64:
            options.arch = 64;
            break;
        default:
            fprintf(stderr, "Unknown architecture: %s\n", arch);
            break;
        }
    }

    if (argv[1][0] != '-')
    {
        handle.path = strduplication(argv[1]);
        handle.args = (char **)memAlloc(sizeof(char *) * argc - 1);

        for (i = 0, p = &argv[1]; i != argc - 1; p++, i++)
        {
            *(handle.args + i) = strduplication(*p);
        }
        *(handle.args + i) = NULL;
        skip_getopt = 1;
    }
    else
    {
        handle.path = strduplication(argv[2]);
        handle.args = (char **)memAlloc(sizeof(char *) * argc - 1);

        for (i = 0, p = &argv[2]; i != argc - 2; p++, i++)
        {
            *(handle.args + i) = strduplication(*p);
        }

        *(handle.args + i) = NULL;
    }

    if (skip_getopt)
        goto debut;

    while ((opt = getopt(argc, argv, "he")) != -1)
    {
        switch (opt)
        {
        case 'e':
            options.elfinfo++;
            break;
        case 'h':
            goto Menu;
        default:
            printf(B_YELLOW);
            printf("UNKNOWN option\n");
            printf(reset);
            exit(0);
        }

    debut:

        

        if ((pid = fork()) < 0)
        {
            perror("fork");
            exit(-1);
        }

        if (pid == 0)
        {
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
            execve(handle.path, handle.args, envp);
            exit(0);
        }

        while (1)
        {
            waitpid(pid, &status, 0);
                    

            if (WIFSIGNALED(status))
            {
                printf(B_RED "Child killed by signal %d (%s)\n" reset, WTERMSIG(status), strsignal(WTERMSIG(status)));
                break;
            }
            else if (WIFSTOPPED(status))
            {
                printf(B_RED "Child stopped by signal %d (%s)\n" reset, WSTOPSIG(status), strsignal(WSTOPSIG(status)));

                switch (WSTOPSIG(status))
                {
                case SIGTRAP:
                    ptrace(PTRACE_CONT, pid, 0, 0);
                    break;

                default:                  
                    sa.sa_sigaction = &handler;
                    sa.sa_flags = SA_SIGINFO;
                    sigaction(WSTOPSIG(status), &sa, NULL);
                    goto analyze_begin;
                }
            }
        }
    }

analyze_begin:
    handle.pid = pid;
    PID = pid;
    printf(B_BLUE "=================================================================\n" reset);
    printf(B_RED "                 Début de debuggage \n" reset);
    printf(B_BLUE "=================================================================\n" reset);
    processes_analyze(&handle);
    goto fin;

fin:
    ptrace(PTRACE_DETACH, handle.pid, NULL, NULL);
    exit(0);
}