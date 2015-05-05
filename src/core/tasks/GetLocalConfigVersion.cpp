//
// Created by xaqq on 5/5/15.
//

#include <tools/log.hpp>
#include <tools/gettid.hpp>
#include "GetLocalConfigVersion.h"

Leosac::Tasks::GetLocalConfigVersion::GetLocalConfigVersion()
{

}

void Leosac::Tasks::GetLocalConfigVersion::do_run()
{
    DEBUG("I AM IN MAIN THREAD. I GUESS. Thread = "<< Leosac::gettid());
}
