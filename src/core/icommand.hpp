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
    typedef std::shared_ptr<ICommand> Ptr;

public:
    virtual ~ICommand() {}
    virtual void    execute() = 0;
};

#endif // ICOMMAND_HPP
