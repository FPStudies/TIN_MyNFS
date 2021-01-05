/**
 * @file clientapi.h
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <iostream>
#include <map>
#include <cstring>
#include <fcntl.h>
#include <string>
#include "clientprotocol.h"
#include "datagrams.h"
#include "LogUtils.h"

extern int mynfs_errno;

enum class MyNFS_ERRORS {
        NONE = 0,
        einval = -2, // Wartość podana jako tryb otwarcia jest niepoprawna.
        enotdir = -3, // Element prefiksu ścieżki nie jest katalogiem.
        etran = -4, // Błąd przesyłania pakietów. Funkcja nie została wykonana.
        econn = -5, // Błąd połączenia. Nie można połączyć się z danym hostem pod danym portem.
        emfile = -6, // Osiągnięta została już maksymalna liczba {OPEN_MAX} deskryptorów plików dla danego użytkownika.
        enametoolong = -7, // Długość ścieżki przekroczyła 	dozwoloną wartość {PATH_MAX} lub nazwa elementu ścieżki przekroczyła długość {NAME_MAX}.
        enfile = -8, // Została już osiągnięta maksymalna liczba otwartych plików/katalogów w systemie.
        enoent = -9, // Wskazany przez ścieżkę plik/katalog nie istnieje bądź ścieżka jest łańcuchem pustym.
        econref = -10, // Odmówiono połączenia pod danym portem.
        encat = -11, // Podany suffix ścieżki nie jest katalogiem.
        ebadf = -12, // Argument mynfs_fd/dirfd nie  jest poprawnym deskryptorem pliku wskazującym na otwarty plik/katalog.
        eio = -13, // Wystąpił błąd wejścia/wyjścia.
        eisdir = -14, // Argument path odnosi się do katalogu.
        eacces = -15, // Brak uprawnień do przeprowadzenia operacji na pliku.
        eagain = -16, // Deskryptor pliku mynfs_fs odnosi się do pliku lub gniazda oznaczonego jako nieblokujące (nonblocking), a próba zapisu by go zablokowała.
        efault = -17, // Bufor buf znajduje się poza dostępną przestrzenią adresową.
        efbig = -18, // Próba zapisu przekraczająca limit rozmiaru pliku lub przekraczająca jego offset.
        eserv = -19, // Niemożność wykonania danej operacji. Błąd serwera. Przypadek braku wystarczającej ilości pamięci lub innych problemów.
        erwsize = -20, // Przekroczenie limit odczytu/zapisu MY_RW_SIZE_MAX.
        enospc = -21, // Urządzenie powiązane z deskryptorem pliku mynfs_fd nie posiada miejsca na nowe dane.
        epipe = -22, // Deskryptor powiązany jest z potokiem lub gniazdem, którego odczyt jest zamknięty.
        eoverflow = -23, //Zwracana wartość przesunięcia nie może być reprezentowana jako typ off_t.
        edevnospc = -24, // Odmowa przyjęcia żądanej ilości bajtów
        etout = -25, // Nastąpił timeout połączenia. Podana operacja nie może zostać wykonana.
        enotfile = -26, // Podany deskryptor nie wskazuje na plik.
    };

struct mynfs_stat
{
	bool		nfs_st_valid;		/* poprawność pobrania atrybutów */
	int32_t		nfs_st_size;	    /* wielkość w bajtach (32 bity z 64) */
	int32_t    	nfs_st_atime;	    /* czas ostatniego dostępu (32 bity) */
	int32_t    	nfs_st_mtime;		/* czas ostatniej modyfikacji (32 bity) */
};

enum class ApiIDS: char
{
    OPEN = 2,
    READ,
    WRITE,
    LSEEK,
    CLOSE,
    UNLINK,
    FSTAT,
    OPENDIR,
    READDIR,
    CLOSEDIR
};

class ClientApi
{
public:  
    ClientApi();

    int mynfs_open(char * host, char * path, int oflag, int mode);
    int mynfs_read(int mynfs_fd, char * buf, int len);
    int mynfs_write(int mynfs_fd, const char * buf, int len);
    int mynfs_lseek(int mynfs_fd, int whence, int offset);
    int mynfs_close(int mynfs_fd);
    int mynfs_closedir(int dirfd);
    char * mynfs_readdir(int dirfd);
    int mynfs_opendir(char *host, char *path);
    mynfs_stat mynfs_fstat(int mynfs_fd);

private:
    std::map<int, Client*> clients;

    void setErrno(int errorID);
    bool clientExist(int fd);
};