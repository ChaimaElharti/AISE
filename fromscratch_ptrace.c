

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <elf.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/reg.h>
#include <stdarg.h> // pour permettre de passer un nombre indéfini d'argument.

/**
 * @brief ANSI COLORS
 * Il y a 8 couleurs ANSI qui peut monter à 16 selon l'affichage
 * gras ou standard
 *
 * **/
#include "../include/ANSI-color-codes.h"

#define MAX_SYMS 8192 * 2

#define FTRACE_ENV "FTRACE_ARCH"

#define MAX_ADDR_SPACE 256

#define TEXT_SPACE 0
#define DATA_SPACE 1
#define STACK_SPACE 2
#define HEAP_SPACE 3

#define CALLSTACK_DEPTH 0xf4240

struct brach_instr
{
    char *mnemonic;
    uint8_t opcode;
};

/*
 *Caratérise une section du elf par son nom, la taille en octet
 l'addresse du début de la section.
*/
struct elf_section_range
{
    char *sh_name;
    unsigned long sh_addr;
    unsigned int sh_size;
};

/*
 *
 */
struct
{
    int elfinfo;
    int typeinfo;
    int getstr;
    int arch;
    int clflow;
    int verbose;
} opts;

/*
 * Structure d'un elf sur architecture 64bit
 */

struct elf64
{
    Elf64_Ehdr *ehdr; //
    Elf64_Phdr *phdr; // programm header table
    Elf64_Shdr *shdr; // Section headers table
    Elf64_Sym *sym;   // symbol table
    Elf64_Dyn *dyn;   //

    char *StringTable;    //
    char *SymStringTable; //
};

struct elf32
{
    Elf32_Ehdr *ehdr; //
    Elf32_Phdr *phdr; /* programm header table */
    Elf32_Shdr *shdr; /* section header table */
    Elf32_Sym *sym;   /* symbol table */
    Elf32_Dyn *dyn;   /*  */

    char *StringTable;    /*  */
    char *SymStringTable; /*  */
};

struct address_space
{
    unsigned long svaddr;
    unsigned long avaddr;
    unsigned int size;
    int count;
};

/*
 * caractérisation d'un symbol par son nom et sa valeur
 */
struct syms
{
    char *name;
    int type;
    unsigned long value;
};

#define MAX_SHDRS 256

/*
 *
 */

struct handle
{
    char *path;
    char **args;
    uint8_t *map;
    struct elf32 *elf32;                          /* Tableau de structures elf32 */
    struct elf64 *elf64;                          /* Tableau de structures elf64 */
    struct elf_section_range sh_range[MAX_SHDRS]; /* Tableau contenant toute les sections */
    struct syms locsyms[MAX_SYMS];                /* Tableau pour stocker les symboles locaux */
    struct syms dynsyms[MAX_SYMS];                /* Tableau pour stocker les symboles dynamiques */
    char *libnames[256];                          /* Tableau pour stocker le nom des libairies charger */
    int loccount;                                 /* compteur du nombre de symbole local */
    int dyncount;                                 /* compteur du nombre de symbole dynamique */
    int libcount;                                 /* compteur du nombre de libairie dynamique */
    int shdr_count;                               /* compteur du nombre de section */
    int pid;
};

pid_t global_pid; /* pid du processus principal */

void load_elf_section_range(struct handle *);
void get_address_space(struct address_space *, int, char *);
void MapElf32(struct handle *); /* fonction pour le mapping en mémoire du fichier elf 32bit */

void MapElf64(struct handle *);                             /* fonction pour le mapping en mémoire du fichier elf 64bit */
char *strduplication(const char *);                         /* fonction pour la duplication d'une chaine de caractères */
char *get_section_by_range(struct handle *, unsigned long); /* */

/**
 * @brief duplication d'une chaine de caratère donnée
 *
 * @param original pointeur sur la chaine de caratère à dupliquer
 * @return char*  ponteur sur la chaine de caractère dupliquée.
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
    // free(p);
}

/**
 * @brief construction d'un pseudo-elf pour les besoins du déboggage
 *
 * @param h
 * @return int retourne 0 si tout se passe bien
 */
int BuildSym(struct handle *h)
{
    unsigned int i, j, k;
    char *symStrTable;
    Elf32_Ehdr *ehdr32;
    Elf32_Shdr *shdr32; /* section header */
    Elf32_Sym *symtab32;

    Elf64_Ehdr *ehdr64;
    Elf64_Shdr *shdr64;
    Elf64_Sym *symtab64;

    int st_type;
    int st_bind;

    h->loccount = 0;
    h->dyncount = 0;

    switch (opts.arch)
    {
    case 32:
        ehdr32 = h->elf32->ehdr; /* ELF header */
        shdr32 = h->elf32->shdr; /* section header */

        /*

         *Sachant qu'on a e_shnum sections,
         *parcourir chaque section puis identifié la section "(SHT_SYMTAB)" ou "SHT_SYNSYM"
         * recupérer les informations
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
                    st_bind = ELF32_ST_BIND(symtab32->st_info); /* bindinding : WEAK, LOCAL, GLOBAL , etc..*/

                    if(st_type != STT_FUNC)
                        continue;

                    switch (shdr32[i].sh_type)
                    {
                    case SHT_SYMTAB:
                        h->locsyms[h->loccount].name = strduplication(&symStrTable[symtab32->st_name]);
                        h->locsyms[h->loccount].value = symtab32->st_value;
                        h->locsyms[h->loccount].type = st_type;
                        h->loccount++;
                        break;
                    case SHT_DYNSYM:
                        h->dynsyms[h->dyncount].name = strduplication(&symStrTable[symtab32->st_name]);
                        h->dynsyms[h->dyncount].value = symtab32->st_value;
                        h->dynsyms[h->dyncount].type = st_type;
                        h->dyncount++;
                        break;
                    }
                }
            }
        }
        h->elf32->StringTable = (char *)&h->map[shdr32[ehdr32->e_shstrndx].sh_offset];
        for (i = 0; i < ehdr32->e_shnum; i++)
        {
            if (!strcmp(&h->elf32->StringTable[shdr32[i].sh_name], ".plt"))
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
                        // h->locsyms[h->loccount].type = st_type;
                        h->loccount++;
                        break;
                    case SHT_DYNSYM:
                        h->dynsyms[h->dyncount].name = strduplication(&symStrTable[symtab64->st_name]);
                        h->dynsyms[h->dyncount].value = symtab64->st_value;
                        // h->dynsyms[h->dyncount].type = st_type;
                        h->dyncount++;
                        break;
                    }
                }
            }
        }
        h->elf64->StringTable = (char *)&h->map[shdr64[ehdr64->e_shstrndx].sh_offset];
        for (i = 0; i < ehdr64->e_shnum; i++)
        {
            if (!strcmp(&h->elf64->StringTable[shdr64[i].sh_name], ".plt"))
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

/* localiser le segment dynamique propre aux fichier faisant appel à des libraries
 * dont la résolution de liens se fait dynamiquement
 */
void locate_dynamic_segment(struct handle *h)
{
    int i;
    switch (opts.arch)
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

uint8_t *get_section_data(struct handle *h, const char *section_name)
{

    char *StringTable;
    int i;

    switch (opts.arch)
    {
    case 32:
        StringTable = h->elf32->StringTable;
        for (i = 0; i < h->elf32->ehdr->e_shnum; i++)
        {
            if (!strcmp(&StringTable[h->elf32->shdr[i].sh_name], section_name))
            {
                return &h->map[h->elf32->shdr[i].sh_offset];
            }
        }
        break;
    case 64:
        StringTable = h->elf64->StringTable;
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

char *get_dt_strtab_name(struct handle *h, int xset)
{
    static char *dyn_strtbl;

    if (!dyn_strtbl && !(dyn_strtbl = get_section_data(h, ".dynstr")))
        printf("[!] Could not locate .dynstr section\n");

    return dyn_strtbl + xset;
}

void parse_dynamic_dt_needed(struct handle *h)
{
    char *symstr;
    int i, n_entries;
    Elf32_Dyn *dyn32;
    Elf64_Dyn *dyn64;

    locate_dynamic_segment(h);
    h->libcount = 0;

    switch (opts.arch)
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
 * @brief
 *
 * @param len
 * @return void*
 */
void *HeapAlloc(unsigned int len)
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
 * @brief
 *
 */
void load_elf_section_range(struct handle *h)
{
    Elf32_Ehdr *ehdr32;
    Elf32_Shdr *shdr32;
    Elf64_Ehdr *ehdr64;
    Elf64_Shdr *shdr64;

    char *StringTable;
    int i;

    h->shdr_count = 0;
    switch (opts.arch)
    {
    case 32:
        StringTable = h->elf32->StringTable;
        ehdr32 = h->elf32->ehdr;
        shdr32 = h->elf32->shdr;

        for (i = 0; i < ehdr32->e_shnum; i++)
        {
            h->sh_range[i].sh_name = strduplication(&StringTable[shdr32[i].sh_name]);
            h->sh_range[i].sh_addr = shdr32[i].sh_addr;
            h->sh_range[i].sh_size = shdr32[i].sh_size;
            if (h->shdr_count == MAX_SHDRS)
                break;
            h->shdr_count++;
        }
        break;
    case 64:
        StringTable = h->elf64->StringTable;
        ehdr64 = h->elf64->ehdr;
        shdr64 = h->elf64->shdr;

        for (i = 0; i < ehdr64->e_shnum; i++)
        {
            h->sh_range[i].sh_name = strduplication(&StringTable[shdr64[i].sh_name]);
            h->sh_range[i].sh_addr = shdr64[i].sh_addr;
            h->sh_range[i].sh_size = shdr64[i].sh_size;
            if (h->shdr_count == MAX_SHDRS)
                break;
            h->shdr_count++;
        }
        break;
    }
}

/**
 * @brief Get the section by range object
 *
 * @param h
 * @param addrs
 * @return char*
 */

char *get_section_by_range(struct handle *h, unsigned long addrs)
{
    int i;

    for (i = 0; i < h->shdr_count; i++)
    {
        if (addrs >= h->sh_range[i].sh_addr && addrs <= h->sh_range[i].sh_addr + h->sh_range[i].sh_size)
            return h->sh_range[i].sh_name;
    }

    return NULL;
}

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

    h->elf32->StringTable = (char *)&h->map[h->elf32->shdr[h->elf32->ehdr->e_shstrndx].sh_offset];

    if (h->elf32->ehdr->e_shnum > 0 && h->elf32->ehdr->e_shstrndx != SHN_UNDEF)
        load_elf_section_range(h);
}

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

    h->elf64->StringTable = (char *)&h->map[h->elf64->shdr[h->elf64->ehdr->e_shstrndx].sh_offset];

    if (h->elf64->ehdr->e_shnum > 0 && h->elf64->ehdr->e_shstrndx != SHN_UNDEF)
        load_elf_section_range(h);
}

/**
 * @brief
 *
 * @param h
 */
void examine_process(struct handle *h)
{
    int i;

    struct address_space *addrspace = (struct address_space *)HeapAlloc(sizeof(struct address_space) * MAX_ADDR_SPACE);

    /*
    Allocation mémoire pour la structure ELF selon
    que l'on soit sur une architecture 32 ou 64,
    mapper ensuite l'éxécutable à cet addresse.
    */
    switch (opts.arch)
    {
    case 32:
        h->elf32 = HeapAlloc(sizeof(struct elf32));
        h->elf64 = NULL;
        MapElf32(h);
        break;
    case 64:
        h->elf64 = HeapAlloc(sizeof(struct elf64));
        h->elf32 = NULL;
        MapElf64(h);
        break;
    }

    // construction information  relative aux symboles
    BuildSym(h);

    // get_address_space((struct address_space *)addrspace, h->pid, h->path);

    if (opts.elfinfo)
    {
        printf(B_BLUE);
        printf("\n[+ @@ +] Information des tables de symbole du programme :\n\n");
        printf(reset);

        for (i = 0; i < h->loccount; i++)
        {
            if (h->locsyms[i].name == NULL)
            {
                printf(B_YELLOW);
                printf("UNKNOMN: 0x%lx\n", h->locsyms[i].value);
                printf(reset);
            }
            else
            {
                printf("%s -> ", h->locsyms[i].name);

                printf(B_GREEN "0x%lx\n", h->locsyms[i].value);

                printf(reset);
                // printf("%s 0x%lx\n", h->locsyms[i].name, h->locsyms[i].value);
            }
        }

        for (i = 0; i < h->dyncount; i++)
        {
            if (h->dynsyms[i].name == NULL)
            {
                printf(B_YELLOW);
                printf("UNKNOWN: 0x%lx\n", h->dynsyms[i].value); // dynsysms <->locsyms
                printf(reset);
            }
            else
            {
                printf("%s -> ", h->dynsyms[i].name);
                printf(B_RED "0x%lx\n", h->dynsyms[i].value);
                printf(reset);
                // printf("%s 0x%lx\n", h->dynsyms[i].name, h->dynsyms[i].value);
            }
        }
        printf(B_BLUE);
        printf("\n[+ @@ +] Bibliothèques dynamiques:\n\n");
        printf(reset);
        parse_dynamic_dt_needed(h);

        for (i = 0; i < h->libcount; i++)
        {
            printf("[%d]\t%s\n", i + 1, h->libnames[i]);
        }
    }
}

int main(int argc, char *argv[], char *envp[])
{

    int i, opt, pid, status, skip_getopt = 0;
    char **p, *arch;
    struct handle handle;

    if (argc < 2)
    {

    Menu:
        printf(B_GREEN);
        printf("Menu: %s [options entre:-Sstve]<prog>\n", argv[0]);
        // printf("[-p] Tracer grace au PID\n");
        printf("[-t] Type des arguments des fonctions\n");
        printf("[-s] Afficher les valeurs de types string\n");
        printf("[-v] Verbose output\n");
        printf("[-e] Misc. ELF info. (Symbols, Dependencies)\n");
        // printf("[-S] Show function calls with stripped symbols\n");
        // printf("[-C] Complete control flow analysis\n");
        printf("[-h] Help");
        printf(reset);
        exit(0);
    }
    if (argc == 2 && argv[1][0] == '-')
        goto Menu;

    // remplir la zone mémoire occupée par opts par des 0

    memset(&opts, 0, sizeof(opts));

    // initialisation de l'architecture par défaut à 64
    opts.arch = 64;
    arch = getenv(FTRACE_ENV);
    if (arch != NULL)
    {
        switch (atoi(arch))
        {
        case 32:
            opts.arch = 32;
            break;
        case 64:
            opts.arch = 64;
            break;
        default:
            fprintf(stderr, "Unknown architecture: %s\n", arch);
            break;
        }
    }

    if (argv[1][0] != '-')
    {
        handle.path = strduplication(argv[1]);
        handle.args = (char **)HeapAlloc(sizeof(char *) * argc - 1);

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
        handle.args = (char **)HeapAlloc(sizeof(char *) * argc - 1);

        for (i = 0, p = &argv[2]; i != argc - 2; p++, i++)
        {
            *(handle.args + i) = strduplication(*p);
        }

        *(handle.args + i) = NULL;
    }

    if (skip_getopt)
        goto debut;

    while ((opt = getopt(argc, argv, "CShtvep:s")) != -1)
    {
        switch (opt)
        {
        case 'e':
            opts.elfinfo++;
            break;
        case 't':
            opts.typeinfo++;
            break;
        case 's':
            opts.getstr;
            break;
        case 'C':
            opts.clflow;
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
        if (opts.verbose)
        {
            switch (opts.arch)
            {
            case 32:
                printf(B_BLUE);
                printf("[+ @@ +] mode ELF 32bit activé!\n");
                printf(reset);
                break;
            case 64:
                printf(B_BLUE);
                printf("[+ @@ +] mode ELF 64bit activé!\n");
                printf(reset);
                break;
            }
            if (opts.typeinfo)
                printf(B_BLUE);
            printf("[+ @@ +] Pointer sur prediction type activé\n");
            printf(reset);
        }
        if (opts.arch == 32 && opts.typeinfo)
        {
            printf(B_RED);
            printf("[!] Option -t ne doit pas être utilisé pour le mode ELF 32bit\n");
            printf(B_RED);
            exit(0);
        }

        if (opts.arch == 32 && opts.getstr)
        {
            printf(B_RED);
            printf("[!] Option -s ne doit pas être utilisé pour le mode ELF 32bit\n");
            printf(B_RED);
            exit(0);
        }

        if (opts.getstr && opts.typeinfo)
        {
            printf(B_RED);
            printf("[!] Options -t et -s ne doivent pas être utilisé ensemble\n");
            printf(B_RED);
            exit(0);
        }

        if ((pid = fork()) < 0)
        {
            perror("fork");
            exit(-1);
        }

        if (pid == 0)
        {
            if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
            {
                perror("PTRACE_TRACEME");
                exit(-1);
            }

            ptrace(PTRACE_SETOPTIONS, 0, 0, PTRACE_O_TRACEEXIT);
            execve(handle.path, handle.args, envp);
            exit(0);
        }

        waitpid(0, &status, WNOHANG);
        handle.pid = pid;
        global_pid = pid;
        examine_process(&handle);
        goto fin;
    }
fin:
    // printf("%s\n", B_WHITE);
    ptrace(PTRACE_DETACH, handle.pid, NULL, NULL);
    exit(0);
}
