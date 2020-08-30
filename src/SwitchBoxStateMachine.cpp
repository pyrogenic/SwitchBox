#include <>
#include <FunctionFSM.h>
#include "SwitchBox.h"

//fsm triggers
enum Trigger {
  TOGGLE_SWITCH
  //You can add more triggers here...
};

typedef struct
{
    std::string inputLabel;
} SwitchBoxState;

SwitchBoxState publicState = { 
};

//fsm state functions
void state_input_a_on_enter() {
  digitalWrite(RELAY_INPUT, LOW);

}

void state_input_b_on_enter() {
  digitalWrite(RELAY_INPUT, HIGH);
}

//fsm states
//define states with FunctionState state(&func_on_enter_state, &func_in_state, &func_on_exit_state)
//you can use nullptr where no function is required

FunctionState state_input_a(&state_input_a_on_enter, nullptr, nullptr);
FunctionState state_input_b(&state_input_b_on_enter, nullptr, nullptr);
//You can have as many states as you'd like


//fsm
//define the fsm with the state it will start in

FunctionFsm fsm(&state_a);
//You can run as many state machines as you'd like


void initfsm() {
//define transitions between states with
//myfsm.add_transition(&state_to_transition_from, &state_to_transition_to, &trigger, &func_on_transition
//you can use nullptr where no transition function is required

//it's useful to group all the add_transitions together so they can be easily called as needed

  fsm.add_transition(&state_a, &state_b, TOGGLE_SWITCH, nullptr);
  fsm.add_transition(&state_b, &state_a, TOGGLE_SWITCH, nullptr);

}

