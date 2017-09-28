/*
    Copyright (C) 2014-2016 Leosac

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

#include "hardware/facades/FGPIO.hpp"
#include "hardware/facades/FLED.hpp"
#include "tools/log.hpp"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{
namespace SM
{
/********************************************
* Define events used in the state machine   *
********************************************/

/**
* Fired when we update the state machine.
*/
struct EventUpdate
{
};

/**
* This event is fired when we want to start blinking.
*/
struct EventBlink
{
    /**
    * Requested duration (in ms) of the blink
    */
    int64_t duration;

    /**
    * Period for before blinking (ie switch state every N ms)
    */
    int64_t speed;
};

/**
* Fired when playing pattern has terminated.
*/
struct EventPlayingPatternStop
{
};

/**
* Fired when we want to "play" a pattern
*/
struct EventPlayingPattern
{
    /**
    * A vector of <duration, speed> that represents
    * our blinking pattern.
    *
    * @note Pattern are played in reverse order.
    */
    std::vector<std::pair<int, int>> pattern;
};

/**
* Fired when we are done blinking
*/
struct EventBlinkStop
{
};

namespace
{
namespace msmf = boost::msm::front;
}

struct LedBuzzerSM_ : msmf::state_machine_def<LedBuzzerSM_>
{

    LedBuzzerSM_(Hardware::FGPIO &h)
        : playing_pattern_(false)
        , gpio_(h)
        , next_update_time_(std::chrono::system_clock::time_point::max())
    {
    }

    struct Idle : msmf::state<>
    {
        // Entry action
        template <class Event, class Fsm>
        void on_entry(Event const &, Fsm &fsm)
        {
            DEBUG("Idle::on_entry()");
            fsm.next_update_time_ = std::chrono::system_clock::time_point::max();
        }

        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const &, Fsm &)
        {
            DEBUG("Idle::on_exit()");
        }
    };

    // We want a predefined blinking pattern
    struct PlayingPattern : msmf::state<>
    {
        // Entry action
        template <class Event, class Fsm>
        void on_entry(Event const &, Fsm &)
        {
            ERROR("Bad event when entering PlayingPattern");
            assert(0);
        }

        template <class Fsm>
        void on_entry(EventBlinkStop const &, Fsm &)
        {
            // we are coming back to PlayingPattern after finishing 1 round of
            // blinking.
        }

        template <class Fsm>
        void on_entry(EventPlayingPattern const &e, Fsm &fsm)
        {
            std::cout << "PlayingPattern::on_entry()" << std::endl;
            fsm.next_update_time_ = std::chrono::system_clock::now();
            fsm.playing_pattern_  = true;
            pattern_              = e.pattern;
        }

        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const &, Fsm &fsm)
        {
            std::cout << "PlayingPattern::on_exit()" << std::endl;
            // if we exit to go into blinking we must not clear the playing_pattern
            // flag
            // we clear only if we exit due to EventPlayingPatternStop
            if (std::is_same<Event, EventPlayingPatternStop>::value)
                fsm.playing_pattern_ = false;
        }

        std::vector<std::pair<int, int>> pattern_;
    };

    struct StateBlinking : msmf::state<>
    {
        // Entry action
        template <class Event, class Fsm>
        void on_entry(Event const &, Fsm &)
        {
            ERROR("Bad event when entering StateBlinking");
            assert(0);
        }

        template <class Fsm>
        void on_entry(EventBlink const &e, Fsm &fsm)
        {
            DEBUG("StateBlinking::on_entry(). Duration = "
                  << e.duration << ". Speed = " << e.speed);
            fsm.next_update_time_   = std::chrono::system_clock::now();
            nb_itr_                 = e.duration / e.speed;
            speed_                  = e.speed;
            fsm.led_state_.duration = e.duration;
            fsm.led_state_.speed    = e.speed;
            fsm.led_state_.st       = Hardware::FLED::State::BLINKING;
        }

        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const &, Fsm &fsm)
        {
            std::cout << "StateBlinking::on_exit()" << std::endl;
            // we don't know nor care what is real state. We just care
            // that it's not blinking anymore.
            fsm.led_state_.st = Hardware::FLED::State::UNKNOWN;
        }

        int speed_;
        int nb_itr_;
    };

    // Set initial state
    typedef Idle initial_state;

    struct UpdateAction
    {
        template <class Event, class Fsm, class SourceState, class TargetState>
        void operator()(Event const &, Fsm &, SourceState &, TargetState &) const
        {
            ERROR("Bad transition");
            assert(0);
        }

        /**
        * Updating PlayingPattern.
        *
        * Chose to stop or setup a blinking behavior by firing an event.
        */
        template <class Event, class Fsm, class TargetState>
        void operator()(Event const &, Fsm &fsm, PlayingPattern &ss,
                        TargetState &) const
        {
            if (ss.pattern_.size() == 0)
            {
                fsm.process_event(EventPlayingPatternStop());
                return;
            }
            std::pair<int, int> current_pattern = ss.pattern_.back();
            ss.pattern_.pop_back();

            // prepare blinking event, then fire it
            EventBlink b;
            b.duration = current_pattern.first;
            b.speed    = current_pattern.second;
            fsm.process_event(b);
        }

        /**
        * Blink by toggling the GPIO, or return to Idle state.
        */
        template <class Event, class Fsm, class TargetState>
        void operator()(Event const &, Fsm &fsm, StateBlinking &ss,
                        TargetState &) const
        {
            fsm.gpio_.toggle();
            fsm.next_update_time_ = std::chrono::system_clock::now() +
                                    std::chrono::milliseconds(ss.speed_);
            if (--ss.nb_itr_ == 0)
            {
                fsm.process_event(EventBlinkStop());
            }
        }
    };

    /**
    * A transition guard that check EventBlinkStop.
    *
    * It choses which state we go to after receiving EventBlinkStop.
    * Either we go back to EventPlayingPattern or to Idle.
    */
    struct transition_guard
    {
        template <class Fsm, class SourceState, class TargetState>
        bool operator()(const EventBlinkStop &, Fsm &fsm, SourceState &,
                        TargetState &)
        {
            if (std::is_same<TargetState, PlayingPattern>::value)
            {
                return fsm.playing_pattern_;
            }
            else
            {
                return !fsm.playing_pattern_;
            }
        }
    };

    // Transition table
    struct transition_table
        : boost::mpl::vector<
              //    Start          Event           Next           Action Guard
              msmf::Row<Idle, EventBlink, StateBlinking, msmf::none, msmf::none>,
              msmf::Row<StateBlinking, EventBlinkStop, Idle, msmf::none,
                        transition_guard>,
              msmf::Row<StateBlinking, EventBlinkStop, PlayingPattern, msmf::none,
                        transition_guard>,
              msmf::Row<StateBlinking, EventUpdate, msmf::none, UpdateAction,
                        msmf::none>,
              msmf::Row<Idle, EventPlayingPattern, PlayingPattern, msmf::none,
                        msmf::none>,
              msmf::Row<PlayingPattern, EventUpdate, msmf::none, UpdateAction,
                        msmf::none>,
              msmf::Row<PlayingPattern, EventBlink, StateBlinking, msmf::none,
                        msmf::none>,
              msmf::Row<PlayingPattern, EventPlayingPatternStop, Idle, msmf::none,
                        msmf::none>>
    {
    };

    std::chrono::system_clock::time_point next_update() const
    {
        return next_update_time_;
    }

    /**
    * If we are currently blinking in pattern, this will be true
    */
    bool playing_pattern_;

    /**
     * The state of the LED.
     * @warning It is not completely accurate ! The `duration` and `speed`
     * are correct, but the `value` is not, since it is not
     * maintained by the state machine.
     */
    Hardware::FLED::State led_state_;


    /**
    * Facade to the underlying gpio
    */
    Hardware::FGPIO &gpio_;

    /**
    * Next time_point we want to be updated.
    * Set to time_point::max() if we don't want to be updated.
    */
    std::chrono::system_clock::time_point next_update_time_;
};
}
typedef boost::msm::back::state_machine<SM::LedBuzzerSM_> LedBuzzerSM;
}
}
}
