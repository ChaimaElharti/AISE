
/**
 * @file dbg.h
 * @author your name (LEODASCE SEWANOU & )
 * @brief 
 * @version 0.1
 * @date 2022-03-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
#include <stdarg.h>
#include <signal.h>

/* Taille maximale pour nombre de symbols*/
#define NB_MAX_SYMBOLS 16364

/* Nombre maximal de sections */
#define NB_MAX_SECTIONS 256

/* Nombre maximal de bibliothèques chargées */
#define NB_MAX_LIB 256

/**
 * @brief Get the status of the process for which PID is given.
 *
 * @param pid
 */
void get_status(int pid);


/**
 * @brief Get the memory maps of the process for which PID is given.
 *
 * @param pid
 */
void get_memory_maps(int pid);


/* structure pour représenter une section elf */
struct elf_section
{
    char *sh_name;
    unsigned long sh_addr;
    unsigned long sh_size;
};

/* structure caractérisant un symbole quelconque */
struct symbol
{
    char *name;
    int type;
    int bind;
    unsigned long value;
};

/*  Structure principale permettant l'exoration du ELF (elf32 ou elf64) */
struct handle
{
    char *path;
    char **args;
    uint8_t *map;
    struct elf32 *elf32;                          /* Tableau de structures elf32 */
    struct elf64 *elf64;                          /* Tableau de structures elf64 */
    struct elf_section sh_range[NB_MAX_SECTIONS]; /* Tableau contenant toutes les sections */
    struct symbol locsyms[NB_MAX_SYMBOLS];        /* Tableau pour stocker les symboles locaux */
    struct symbol dynsyms[NB_MAX_SYMBOLS];        /* Tableau pour stocker les symboles dynamiques */
    char *libnames[256];                          /* Tableau pour stocker le nom des libairies chargées */
    int loccount;                                 /* compteur du nombre de symbole local */
    int dyncount;                                 /* compteur du nombre de symbole dynamique */
    int libcount;                                 /* compteur du nombre de libairie dynamique */
    int shdr_count;
    struct user_regs_struct p_struct; /* compteur du nombre de section */
    int pid;
    char *local;
};


/**
 * @brief Gestionnaire des signaux
 *
 * @param signum numéro signal
 * @param si structure siginfo_t
 * @param context
 */
static void handler(int signum, siginfo_t *si, void *context);


/**
 * @brief Get the Symbol object
 *
 * @param h
 * @return int
 */
int getSymbol(struct handle *h);

/* structure de gestion des options du débuggage */
struct
{
    int elfinfo;
    int typeinfo;
    int getstr;
    int arch;
} options;

/* structure d'un elf au format 64 bit*/
struct elf64
{
    Elf64_Ehdr *ehdr; //
    Elf64_Phdr *phdr; // programm header table
    Elf64_Shdr *shdr; // Section headers table
    Elf64_Sym *sym;   // symbol table
    Elf64_Dyn *dyn;   // dynamique symbol table

    char *stringTable;    // string table
    char *symStringTable; //
};

/* structure d'un  elf au format 32 bit */
struct elf32
{
    Elf32_Ehdr *ehdr; //
    Elf32_Phdr *phdr; // programm header table
    Elf32_Shdr *shdr; // Section headers table
    Elf32_Sym *sym;   // symbol table
    Elf32_Dyn *dyn;   // dynamique symbol table

    char *stringTable;    // string table
    char *symStringtable; //
};



/* */
struct address_space
{
    unsigned long svaddr;
    unsigned long avaddr;
    unsigned int size;
    int count;
};

/* Main PID */
pid_t PID;

/**
 * @brief duplication d'une chaine de caratère donnée
 *
 * @param original pointeur sur la chaine de caratère à dupliquer
 * @return char*  ponteur sur la chaine de caractère dupliquée.
 */
char *strduplication(const char *original);


/* localiser le segment dynamique propre aux fichier faisant appel à des libraries
 * dont la résolution de liens se fait dynamiquement
 */
void locate_dynamic_segment(struct handle *h);


/* alocation mémoire sur le tas selon le besoin*/
void *memAlloc(unsigned int len);

/**
 * @brief Charger le elf en mémoire
 *
 * @param h
 */
void load_elf_section(struct handle *h);

/**
 * @brief Projection en mémoire du elf 32bit
 *
 * @param h
 */
void MapElf32(struct handle *h);

/**
 * @brief Projection en mémoire du elf 64bit
 *
 * @param h
 */
void MapElf64(struct handle *h);

/**
 * @brief avoir l'offset d'une section donnée
 *
 * @param h
 * @param section_name nom de la section dont on cherche l'adresse
 * @return uint8_t*
 */
uint8_t *get_section_data(struct handle *h, const char *section_name);

/**
 * @brief pemert d'identifier une librairie dynamique donnée
 *
 * @param h
 * @param xset  offset
 * @return char* pointeur sur le nom de la lib chargée dynamiquement
 */
char *get_dt_strtab_name(struct handle *h, int xset);


/**
 * @brief Récupération des noms de chaque libraries n et les stocker
 * dans le tableau libnames;
 *
 * @param h
 */
void parse_dynamic_dt_needed(struct handle *h);

/**
 * @brief Analyse du processus [à débugger]
 *
 * @param h
 */
void processes_analyze(struct handle *h);

