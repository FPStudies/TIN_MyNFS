#ifndef FD_MANAGER_H
#define FD_MANAGER_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <mutex>
#include <memory>
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/core/ref.hpp>
#include <functional>
#include "FileDescriptor.h"


class FDManager{
    struct Container{
        int id;
        std::string path;
        FileDescriptor fd;

        Container(FileDescriptor&& fileDescriptor);
        const int& operator()(const std::reference_wrapper<int>& x) const;
    };

    /*struct IdExtractor{
        const int& operator()(const std::unique_ptr<Container>& x) const;
    };

    struct PathExtractor{
        const std::string& operator()(const std::unique_ptr<Container>& x) const;
    };

    template<class One, class Two>
    struct Extractor{
        typedef typename One::result_type result_type;
        Extractor(const One& one = One(), const Two& two = Two())
        : one_(one), two_(two)
        {}


    private:
        One one_;
        Two two_;
    };*/

    struct IndexById {};
    struct IndexByPath {};

    std::mutex monitor_;
    using MultiMap = typename boost::multi_index_container<
        std::unique_ptr<Container>,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<IndexById>,
                boost::multi_index::member<Container, int, &Container::id>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<IndexByPath>,
                boost::multi_index::member<Container, std::string, &Container::path>
            >
        >
    >;
    MultiMap fd_;

public:
    FDManager() = default;
    FDManager(const FDManager& other) = delete;
    FDManager(FDManager&& other) = delete;
    FDManager& operator=(const FDManager& other) = delete;
    FDManager& operator=(FDManager&& oother) = delete;

    bool add(FileDescriptor&& fileDescriptor);
    bool remove(int fd, int pid);
    bool exist(int fd);
    bool exist(int fd, int pid);
    FileDescriptor& get(int fd, int pid);


};

#endif