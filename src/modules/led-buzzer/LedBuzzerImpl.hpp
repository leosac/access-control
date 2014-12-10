#pragma once

#include "tools/log.hpp"
#include <zmqpp/zmqpp.hpp>
#include <chrono>
#include "hardware/FGPIO.hpp"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace Leosac
{
    namespace Module
    {
        namespace LedBuzzer
        {

            // ----- Events
            struct Event1
            {
            };

            struct EventUpdate
            {
            };

            struct EventBlink
            {
	      int duration;
	      int speed;
            };

	  struct EventPattern1Stop
	  {};
	  struct EventPattern1
	  {
	  };
	  
	  struct EventBlinkStop
	  {
	  };

            namespace
            {
                namespace msmf = boost::msm::front;
            }

            struct pattern_msm_ : boost::msm::front::state_machine_def<pattern_msm_>
            {   // States

	      pattern_msm_(Hardware::FGPIO &h):
		gpio_(h),
		next_update_time_(std::chrono::system_clock::time_point::max()),
		is_state_blinking_(false),
		playing_pattern_(false)	       
	      {
	      }

                struct Init : boost::msm::front::state<>
                {
                };

                struct State1 : boost::msm::front::state<>
                {
                    // Entry action
                    template<class Event, class Fsm>
                    void on_entry(Event const &, Fsm &fsm)
                    {
                        std::cout << "State1::on_entry()" << std::endl;
			fsm.next_update_time_ = std::chrono::system_clock::time_point::max();
                    }

                    // Exit action
                    template<class Event, class Fsm>
                    void on_exit(Event const &, Fsm &)
                    {
                        std::cout << "State1::on_exit()" << std::endl;
                    }
                };

	      // We want a predefined blinking pattern
                struct StatePattern1 : boost::msm::front::state<>
                {
                    // Entry action
                    template<class Event, class Fsm>
                    void on_entry(Event const &, Fsm &fsm)
                    {
                        std::cout << "StatePattern1::on_entry()" << std::endl;
			fsm.next_update_time_ = std::chrono::system_clock::now();
			fsm.playing_pattern_ = true;
                    }

                    // Exit action
                    template<class Event, class Fsm>
                    void on_exit(Event const &e, Fsm &fsm)
                    {
                        std::cout << "StatePattern1::on_exit()" << std::endl;
			// if we exit to go into blinking we must not clear the playing_pattern flag
			// we clear only if we exit due to EventPattern1Stop
			if (std::is_same<Event, EventPattern1Stop>::value)
			  fsm.playing_pattern_ = false;
                    }

		  std::vector<std::pair<int, int>> pattern_;
                };


                struct StateBlinking : msmf::state<>
                {
                    // Entry action
                    template<class Event, class Fsm>
                    void on_entry(Event const &, Fsm &)
		  {
		    ERROR("Random event");
		    assert(0);
                    }

                    template<class Fsm>
                    void on_entry(EventBlink const & e, Fsm &fsm)
                    {
                        std::cout << "StateBlinking::on_entry() with event Blink: " << e.duration << std::endl;
			fsm.next_update_time_ = std::chrono::system_clock::now();
			nb_itr_ = e.duration / e.speed;
			speed_ = e.speed;
			fsm.is_state_blinking_ = true;
                    }

                    // Exit action
                    template<class Event, class Fsm>
                    void on_exit(Event const &, Fsm &fsm)
                    {
		      std::cout << "StateBlinking::on_exit()" << std::endl;
		      fsm.is_state_blinking_ = false;
                    }

		  int speed_;
		  int nb_itr_;
                };

                // Set initial state
                typedef Init initial_state;

                // Actions
                struct InitAction
                {
                    template<class Event, class Fsm, class SourceState, class TargetState>
                    void operator()(Event const &, Fsm &, SourceState &, TargetState &) const
                    {
                        std::cout << "InitAction()" << std::endl;
                    }
                };

	      struct ActionInitPattern1
	      {
		/**
		 * Init pattern when we go from State1 to Pattern1
		 */
		template<class Event, class Fsm>
		void operator()(Event const &, Fsm &, State1 &, StatePattern1 &sp) const
		{
		  sp.pattern_ = {
		    {4000, 500},
		    {3000, 100},
		    {000, 1000}
		  };
		}
		
	      };

	      struct TestAction
	      {
                    template<class Event, class Fsm, class SourceState, class TargetState>
		    void operator()(Event const &, Fsm &, SourceState &, TargetState &) const                {
		      ERROR("HERE");
		      assert(0);
		    }
		
		// updateing statepattern1
		template<class Event, class Fsm, class TargetState>
		void operator()(Event const &, Fsm &fsm, StatePattern1 &ss, TargetState &) const
		{
		  INFO("HOHOHO size = " << ss.pattern_.size());
		  if (ss.pattern_.size() == 0)
		    {
		      fsm.process_event(EventPattern1Stop());
		      return;
		    }
		  std::pair<int, int> current_pattern = ss.pattern_.back();
		  ss.pattern_.pop_back();
		  EventBlink b;

		  b.duration = current_pattern.first;
		  b.speed = current_pattern.second;
		  fsm.process_event(b);
		}

		template<class Event, class Fsm, class TargetState>
		void operator()(Event const &, Fsm &fsm, StateBlinking &ss, TargetState &) const
		{
		  INFO("TestAction:: Current nb_itr = " << ss.nb_itr_);
		  fsm.gpio_.toggle();
		  fsm.next_update_time_ = std::chrono::system_clock::now() + std::chrono::milliseconds(ss.speed_);
		  if (--ss.nb_itr_ == 0)
		    {
		      INFO("STOPPING");
		      fsm.process_event(EventBlinkStop());
		    }
		}
	      };

	      struct wtf
	      {
		template<class Fsm, class SourceState, class TargetState>
		bool operator()(const EventBlinkStop &,
				Fsm &fsm,
				SourceState &ss,
				TargetState &ts
				)
		{
		  INFO("GUARD IS CHECKING TRANSITION FROM " <<
		       typeid(ss).name() << " to " <<
		       typeid(ts).name());
		  if (std::is_same<TargetState, StatePattern1>::value)
		    {
		      INFO("Will return: " << fsm.playing_pattern_);
		    return fsm.playing_pattern_;
		    }
		  else
		    {
		      INFO("Will return: " << !fsm.playing_pattern_);
		    return !fsm.playing_pattern_;
		    }
		}
	      };
	      bool test_guard(const EventBlinkStop &)
	      {
		//		return playing_pattern_;
		return false;
	      }

	      typedef pattern_msm_ test;

                // Transition table
                struct transition_table : boost::mpl::vector<
                        //    Start   Event       Next    Action      Guard
		  msmf::Row < Init, msmf::none, State1, InitAction, msmf::none>,
		  msmf::Row < State1, Event1, State1, msmf::none, msmf::none>,
		  msmf::Row < State1, EventBlink, StateBlinking, msmf::none, msmf::none>,
		  //return to state1 after stopping blinking only if blinking wasn't initiated by Pattern1
		  
		  msmf::Row < StateBlinking, EventBlinkStop, State1, msmf::none, wtf>,
		  msmf::Row < StateBlinking, EventBlinkStop, StatePattern1, msmf::none, wtf>,
		  msmf::Row < StateBlinking, EventUpdate, msmf::none, TestAction, msmf::none>,
		  msmf::Row < State1, EventPattern1, StatePattern1, ActionInitPattern1, msmf::none>,
		  msmf::Row < StatePattern1, EventUpdate, msmf::none, TestAction, msmf::none>,
		  msmf::Row < StatePattern1, EventBlink, StateBlinking, msmf::none, msmf::none>,
		  msmf::Row < StatePattern1, EventPattern1Stop, State1, msmf::none, msmf::none>
> {};

	      std::chrono::system_clock::time_point next_update() const
	      {
		return next_update_time_;
	      }

	      bool playing_pattern_;
	      Hardware::FGPIO &gpio_;
	      std::chrono::system_clock::time_point next_update_time_;
	      bool is_state_blinking_;
            };

            typedef boost::msm::back::state_machine<pattern_msm_> MyStateMachine;

            /**
            * Implementation class, for use by the LED module only.
            */
            class LedBuzzerImpl
            {
            public:
                /**
                * @param ctx ZMQ context
                * @param led_name name of the led object
                * @param gpio_name name of the gpio we use to drive this led.
                */
                LedBuzzerImpl(zmqpp::context &ctx,
                        const std::string &led_name,
                        const std::string &gpio_name,
                        int blink_duration,
                        int blink_speed);

                /**
                * Return the `frontend_` socket.
                */
                zmqpp::socket &frontend();

                /**
                * Message received on the `rep_` socket.
                */
                void handle_message();

                /**
                * Time point of the next wanted update.
                * Set to time_point::max() if not wanted.
                */
                std::chrono::system_clock::time_point next_update();

                /**
                * Update the object.
                * Only use case is blinking which simplifies code (`ON` with delay is fully handled by GPIO).
                */
                void update();

            private:
                /**
                * Send a message to the backend object (used for ON, OFF, TOGGLE).
                * Return the response message.
                */
                zmqpp::message send_to_backend(zmqpp::message &msg);

                /**
                * Write the current state of the LED device (according to specs)
                * to the `frontend_` socket.
                */
                void send_state();

                /**
                * Start blinking, this stores the blink_end timepoint and send commands for blinking
                * to happen. Register `update()`.
                */
                bool start_blink(zmqpp::message *msg);

                zmqpp::context &ctx_;

                /**
                * REP socket to receive LED command.
                */
                zmqpp::socket frontend_;

                /**
                * REQ socket to the backend GPIO.
                */
                zmqpp::socket backend_;

                /**
                * Facade to the GPIO we use with this LED.
                * While we send command directly most of the time (through the backend socket), this can be used too.
                */
                Hardware::FGPIO gpio_;

                int64_t default_blink_duration_;
                int64_t default_blink_speed_;

                int64_t blink_speed_;
                int64_t blink_duration_;

                /**
                * Number of toggle left to do. This starts at N and decrements over time. When it reaches 0 it means
                * we can stop toggling the gpio.
                */
                int blink_count_;

                MyStateMachine stmachine_;
            };

        }
    }
}
