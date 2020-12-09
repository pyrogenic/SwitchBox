// This file is part of arduino-fsm.
//
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

#include "Fsm.h"

template <typename Event>
Fsm<Event>::Fsm(const char *name, State &initial_state) : m_name(name), m_initialized(false), m_current_state(initial_state), m_transitions(NULL), m_num_transitions(0), m_num_timed_transitions(0) {
}

template <typename Event> Fsm<Event>::~Fsm() {
  free(m_transitions);
  free(m_timed_transitions);
  m_transitions = NULL;
  m_timed_transitions = NULL;
}

template <typename Event> void Fsm<Event>::add_transition(State &state_from, State &state_to, Event event, void (*on_transition)()) {
  m_transitions = (Fsm<Event>::Transition *)realloc(m_transitions, (m_num_transitions + 1) * sizeof(Fsm<Event>::Transition));
  auto &transition = m_transitions[m_num_transitions];
  transition.state_from = state_from;
  transition.state_to = state_to;
  transition.event = event;
  transition.on_transition = on_transition;
  m_num_transitions++;
}

template <typename Event> void Fsm<Event>::add_timed_transition(State &state_from, State &state_to, unsigned long interval, void (*on_transition)()) {
  m_timed_transitions = (Fsm<Event>::TimedTransition *)realloc(m_timed_transitions, (m_num_timed_transitions + 1) * sizeof(Fsm<Event>::TimedTransition));
  auto &timed_transition = m_timed_transitions[m_num_timed_transitions];
  timed_transition.transition.state_from = state_from;
  timed_transition.transition.state_to = state_to;
  timed_transition.transition.on_transition = on_transition;
  timed_transition.at = -1;
  timed_transition.interval = interval;
  m_num_timed_transitions++;
}

template <typename Event> const State &Fsm<Event>::get_current_state() const {
  return m_current_state;
}

template <typename Event> void Fsm<Event>::trigger(Event event) {
  if (m_initialized) {
    for (int i = 0; i < m_num_transitions; ++i) {
      if (m_transitions[i].event == event) {
        Fsm<Event>::make_transition(&(m_transitions[i]));
        return;
      }
    }
  }
}

template <typename Event> void Fsm<Event>::check_timed_transitions() {
  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i) {
    Fsm<Event>::TimedTransition *transition = &m_timed_transitions[i];
    if (in(transition->transition.state_from)) {
      if (transition->at == 0) {
        transition->at = now + transition->interval;
      } else if (transition->at <= now) {
        make_transition(&transition->transition);
        transition->at = 0;
      }
    }
  }
}

template <typename Event> void Fsm<Event>::run_machine() {
  // first run must exec first state "on_enter"
  if (!m_initialized) {
    m_initialized = true;
    if (m_current_state.on_enter != NULL)
      m_current_state.on_enter();
  }

  if (m_current_state.on_state != NULL)
    m_current_state.on_state();

  check_timed_transitions();
}

template <typename Event> bool Fsm<Event>::in(const State &state) {
  return &state == &m_current_state;
}

template <typename Event> void Fsm<Event>::make_transition(Fsm<Event>::Transition *transition) {
  // Execute the handlers in the correct order.
  if (transition->state_from.on_exit != NULL) {
    transition->state_from.on_exit();
  }

  if (transition->on_transition != NULL) {
    transition->on_transition();
  }

  if (transition->state_to.on_enter != NULL) {
    transition->state_to.on_enter();
  }

  m_current_state = transition->state_to;

  // Initialize all timed transitions from m_current_state
  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i) {
    auto &timed_transition = m_timed_transitions[i];
    if (in(timed_transition.transition.state_from))
      timed_transition.at = now + timed_transition.interval;
  }
}
