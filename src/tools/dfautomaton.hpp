/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file dfautomaton.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DFAutomaton class declaration
 * NOTE Node marked 0 is used as error node, don't register function calls
 * on this node
 */

#ifndef DFAUTOMATON_HPP
#define DFAUTOMATON_HPP

#include <map>
#include <functional>

#include "log.hpp" // TEST

template <class UserType, class IdxType, class TransitionType>
class DFAutomaton
{
    using TransitionMap = std::map<IdxType, std::map<TransitionType, IdxType> >;

public:
    using Node = std::function<void(UserType&)>;

public:
    explicit DFAutomaton() = default;
    ~DFAutomaton() = default;

    DFAutomaton(const DFAutomaton& other) = delete;
    DFAutomaton& operator=(const DFAutomaton& other) = delete;

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

        INFO("DFA Update: start=" << current << " transition=" << transition << " dest=" << targetIdx);
        if (node)
            node(instance);

        return (targetIdx);
    }

private:
    TransitionMap           _transitionMap;
    std::map<IdxType, Node> _nodeGraph;
};

#endif // DFAUTOMATON_HPP
