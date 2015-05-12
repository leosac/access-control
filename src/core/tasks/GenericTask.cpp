//
// Created by xaqq on 5/11/15.
//

#include "GenericTask.hpp"

Leosac::Tasks::GenericTask::GenericTask(const std::function<bool(void)> &fct) :
        fct_(fct)
{

}

bool Leosac::Tasks::GenericTask::do_run()
{
    return fct_();
}
