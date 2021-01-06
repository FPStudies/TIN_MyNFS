/**
 * @file clienthandler.h
 * @author Mateusz Kordowski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <mutex>
#include <thread>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/range.hpp>
#include <iostream>

#ifndef THREAD_ID_H
#define THREAD_ID_H

/**
 * @brief Klasa służąca do przechowywania mapy systemowego ID wątku z własno wygenerowanym ID.
 * 
 */
class ThreadID{

public:
    static ThreadID& getInstance(){
        static ThreadID instance_;
        return instance_;
    }

    ThreadID(const ThreadID&) = delete;
    ThreadID& operator=(const ThreadID& other) = delete;

private:
    ThreadID() = default;
    ~ThreadID() = default;

    struct Container{
        std::thread::id os_id_;
        int id_;

        Container(std::thread::id osID, int id);
    };

    struct IndexByOSID{};
    struct IndexByID{};

    std::mutex mutex_;
    using MultiMap = typename boost::multi_index_container<
        std::unique_ptr<Container>,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<IndexByOSID>,
                boost::multi_index::member<Container, std::thread::id, &Container::os_id_>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<IndexByID>,
                boost::multi_index::member<Container, int, &Container::id_>
            >
        >
    >;

    MultiMap map_;    

public:

    /**
     * @brief Dodanie 
     * 
     * @param id własne ID, które będzie zmapowane na systemowe ID wątku.
     * @return true 
     * @return false 
     */
    bool add(int id);

    /**
     * @brief Usuwanie
     * 
     * @param id własne ID
     * @return true 
     * @return false 
     */
    bool remove(int id);

    /**
     * @brief Zwraca własne ID wątku względem ID wątku z którego zostało to wywołane
     * @return int 
     */
    int get();
};


#endif