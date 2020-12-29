#ifndef FD_GEN_H
#define FD_GEN_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <mutex>
#include <set>

/**
 * @brief Klasa służy go generacji nowych nieujemnych wartości identyfikatorów. Zaimplementowana
 * jako monitor.
 */
class IDGen{
    using ID = int;
    std::set<ID> values_;
    ID nextPossibleValue_;
    mutable std::mutex monitor_;

    bool isIdBad(ID id) const;

public:
    IDGen(const ID& startValue = 1);

    /**
     * @brief zwraca nowe, nieużywane ID.
     * 
     * @return ID - nowe ID
     */
    ID get();

    /**
     * @brief Sprawdza, czy dane ID istnieje.
     * 
     * @param id 
     * @return true - istnieje
     * @return false - nie istnieje
     */
    bool exist(const ID& id) const;

    /**
     * @brief Umożliwia ponowne użycie danego ID.
     * 
     * @param id 
     * @return true - operacja udana
     * @return false - operacja nieudana
     */
    bool dispose(const ID& id);

};

#endif