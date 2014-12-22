#pragma once

// Forward declarations
namespace Leosac
{
    namespace Auth
    {
        class BaseAuthSource;
        class WiegandCard;
    }
}

namespace Leosac
{
    namespace Tools
    {
        class IVisitable;

        /**
        * Provide an interface from which visitor object can pick methods it
        * wants to reimplement.
        *
        * Each time a new kind of visitor is created, it should be
        * appended to this class.
        * The methods in this interface are actually not pure, because this
        * allows visitor to ignore type they are not interested in.
        *
        * @see Leosac::Tools::IVisitable
        */
        class IVisitor
        {
        public:
            /**
            * Visit a Visitable object.
            */
            virtual void visit(IVisitable *)
            {
            }

            virtual void visit(Leosac::Auth::BaseAuthSource *)
            {
            }

            virtual void visit(Leosac::Auth::WiegandCard *)
            {

            }
        };
    }
}
