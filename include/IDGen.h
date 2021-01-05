#ifndef FD_GEN_H
#define FD_GEN_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <set>
#include <stdexcept>
#include <mutex>
#include "LogUtils.h"
/**
 * @brief Klasa służy go generacji nowych nieujemnych wartości identyfikatorów. Zaimplementowana
 * jako monitor.
 */
class IDGen{
protected:
    using ID = int;
    std::set<ID> values_;
    ID nextPossibleValue_;
    ID startVal_;
    ID endVal_;

    bool isIdBad(ID id) const;

public:
    IDGen(const ID& startValue = 1, const ID& maxValue = INT32_MAX - 1);

    /**
     * @brief zwraca nowe, nieużywane ID.
     * 
     * @return ID - nowe ID. wróci INT32_MIN w przypadku braku jakichkolwiek wolnych ID
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

    size_t size() const;

};

class IDGenMonitor: public IDGen{
    mutable std::mutex monitor_;
    
public:
    IDGenMonitor(const ID& startValue = 1, const ID& maxValue = INT32_MAX);

    ID get();
    bool exist(const ID& id) const;
    bool dispose(const ID& id);
    size_t size() const;
};

#endif
