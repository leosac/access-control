#pragma once

namespace Leosac
{
    namespace Tools
    {
        class IVisitor;

        /**
        * Provide an interface that visitable object must implement.
        *
        * @see Leosac::Tools::IVisitor
        */
        class IVisitable
        {
        public:
            /**
            * Accept a visitor.
            * @param Visitor shall inherits from the IVisitor interface.
            *
            * @note This must be reimplemented by **all** subclass.
            */
            virtual void accept(IVisitor *) = 0;
        };
    }
}
