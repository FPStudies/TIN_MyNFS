#ifndef ERRORS_H
#define ERRORS_H

#include <string>

class Error{

public:
    enum class Type{
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

private:
    Type type_;

public:
    Error()
    : type_(Type::NONE)
    {}

    Error(Type type)
    : type_(type)
    {}

    Error(const Error& other) = default;
    Error(Error&& other) = default;
    Error& operator=(const Error& other) = default;
    Error& operator=(Error&& other) = default;
    Error& operator=(Type type) {
        type_ = type;
        return *this;
    }

    Type get() const {return type_;};
    void set(Type type) {type_ = type;}
    int getInt() const {return static_cast<int>(type_);}

    static int toInt(Type error){
        return static_cast<int>(error);
    }

    static std::string toString(Type type){
        switch(type){
            case Type::NONE:
                return "NONE";
            case Type::einval:
                return "einval";
            case Type::enotdir:
                return "enotdir";
            case Type::etran:
                return "etran";
            case Type::econn:
                return "econn";
            case Type::emfile:
                return "emfile";
            case Type::enametoolong:
                return "enametoolong";
            case Type::enfile:
                return "enfile";
            case Type::enoent:
                return "enoent";
            case Type::econref:
                return "econref";
            case Type::encat:
                return "encat";
            case Type::ebadf:
                return "ebadf";
            case Type::eio:
                return "eio";
            case Type::eisdir:
                return "eisdir";
            case Type::eacces:
                return "eacces";
            case Type::eagain:
                return "eagain";
            case Type::efault:
                return "efault";
            case Type::efbig:
                return "efbig";
            case Type::eserv:
                return "eserv";
            case Type::erwsize:
                return "erwsize";
            case Type::enospc:
                return "enospc";
            case Type::epipe:
                return "epipe";
            case Type::eoverflow:
                return "eoverflow";
            case Type::edevnospc:
                return "edevnospc";
            case Type::etout:
                return "etout";
            case Type::enotfile:
                return "enotfile";
            default:
                return "NOT IMPLEMENTED";
        }
    }

    operator std::string(){
        return this->toString(this->type_);
    }

};



#endif