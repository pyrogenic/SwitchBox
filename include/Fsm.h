//
// This file is part of arduino-fsm.
// arduino-fsm is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// arduino-fsm is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with arduino-fsm.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Arduino.h>

#define FSM_CALLBACK(name) void (*name)()

typedef struct State {
  State(PROGMEM const char *name, FSM_CALLBACK(on_enter), FSM_CALLBACK(on_state), FSM_CALLBACK(on_exit));
  PROGMEM const char *name;
  FSM_CALLBACK(on_enter);
  FSM_CALLBACK(on_state);
  FSM_CALLBACK(on_exit);
};

template <typename Event> class Fsm {
public:
  Fsm(PROGMEM const char *name, State &initial_state);
  ~Fsm();

  void add_transition(State &state_from, State &state_to, Event event, FSM_CALLBACK(on_transition));

  void add_timed_transition(State &state_from, State &state_to, unsigned long interval, FSM_CALLBACK(on_transition));

  void check_timed_transitions();

  void trigger(Event event);
  bool in(const State &state);
  void run_machine();

  const State &get_current_state() const;

private:
  struct Transition {
    State &state_from;
    State &state_to;
    Event event;
    FSM_CALLBACK(on_transition);
  };

  struct TimedTransition {
    Transition transition;
    unsigned long interval;
    long at;
  };

  void make_transition(Transition *transition);

private:
  PROGMEM const char *m_name;
  bool m_initialized;

  State &m_current_state;
  Transition *m_transitions;
  int m_num_transitions;

  TimedTransition *m_timed_transitions;
  int m_num_timed_transitions;
};
