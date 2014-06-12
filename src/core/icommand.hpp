/**
 * \file icommand.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ICommand interface
 */

#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

class ICommand
{
public:
    virtual ~ICommand() {}
    virtual void    execute() = 0;
};

#endif // ICOMMAND_HPP
