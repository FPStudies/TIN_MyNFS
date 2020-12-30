#ifndef FD_MANAGER_H
#define FD_MANAGER_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <mutex>
#include <memory>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include "FileDescriptor.h"
#include "spdlog/spdlog.h"
#include "LogUtils.h"

class FDManager{
    struct Container{
        int id;
        std::string path;
        FileDescriptor fd;

        Container(FileDescriptor&& fileDescriptor);
        const int& operator()(const std::reference_wrapper<int>& x) const;
    };

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
    bool exist(const std::string& path);
    bool exist(const std::string& path, int pid);
    FileDescriptor& get(int fd, int pid);
    FileDescriptor& get(const std::string& path);


};

#endif