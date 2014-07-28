/**
 * \file icommand.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ICommand interface
 */

#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <memory>

class ICommand
{
public:
    using Ptr = std::shared_ptr<ICommand>;

public:
    virtual ~ICommand() {}
    virtual void    execute() = 0;
};

#endif // ICOMMAND_HPP
