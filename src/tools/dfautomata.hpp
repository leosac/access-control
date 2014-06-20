/**
 * \file dfautomata.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DFAutomata class declaration
 * NOTE Node marked 0 is used as error node, don't register function calls
 * on this node
 */

#ifndef DFAUTOMATA_HPP
#define DFAUTOMATA_HPP

#include <map>
#include <functional>

#include "log.hpp" // TEST

template <class UserType, class IdxType, class TransitionType>
class DFAutomata
{
    typedef std::map<IdxType, std::map<TransitionType, IdxType> > TransitionMap;

public:
    typedef std::function<void(UserType&)> Node;

public:
    explicit DFAutomata() = default;
    ~DFAutomata() = default;

    DFAutomata(const DFAutomata& other) = delete;
    DFAutomata& operator=(const DFAutomata& other) = delete;

public:
    void    addNode(IdxType index, Node node)
    {
        _nodeGraph[index] = node;
    }

    void    addTransition(IdxType start, TransitionType transition, IdxType end)
    {
        _transitionMap[start][transition] = end;
    }

    void    startNode(UserType& instance, IdxType current)
    {
        Node& node = _nodeGraph[current];

        if (node)
            node(instance);
    }

    IdxType update(UserType& instance, IdxType current, TransitionType transition)
    {
        IdxType targetIdx = _transitionMap[current][transition];
        Node& node = _nodeGraph[targetIdx];

        LOG() << "DFA Update: start=" << current << " transition=" << transition << " dest=" << targetIdx;
        if (node)
            node(instance);

        return (targetIdx);
    }

private:
    TransitionMap           _transitionMap;
    std::map<IdxType, Node> _nodeGraph;
};

#endif // DFAUTOMATA_HPP
