//
//  Utils.h
//  Graphoffline
//
//  Created by Anas Rchid on 08/04/2022
//
//

#ifndef UTILS_HPP
#define UTILS_HPP

template <typename Callback> class EndOfScopeExecutor
{
    Callback m_callback;

public:
    EndOfScopeExecutor(const EndOfScopeExecutor &) = delete;
    EndOfScopeExecutor(EndOfScopeExecutor &&) = delete;

    inline explicit EndOfScopeExecutor(Callback &&callback)
        : m_callback(std::forward<Callback>(callback))
    {
    }

    ~EndOfScopeExecutor()
    {
        m_callback();
    }

    EndOfScopeExecutor &operator=(const EndOfScopeExecutor &) = delete;
    EndOfScopeExecutor &operator=(EndOfScopeExecutor &&) = delete;
};

#endif /* UTILS_HPP */
