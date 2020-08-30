#include <string>
#include <Arduino.h>
#include <FunctionFSM.h>
#include "SwitchBox.h"

//fsm triggers
typedef enum Trigger {
  kTriggerToggleInput,
  kTriggerSelectInputA,
  kTriggerSelectInputB,

  kTriggerToggleValhallaPreamp,
  kTriggerSelectValhallaPreampBypass,
  kTriggerSelectValhallaPreampEngage,

  kTriggerToggleOutput,
  kTriggerSelectOutputA,
  kTriggerSelectOutputB,
  kTriggerSelectOutputC,

  kTriggerValhallaBecamePreamp,
  kTriggerValhallaBecameOutput
};

typedef struct
{
    std::string inputLabel;
} SwitchBoxState;

SwitchBoxState publicState = { 
};

void state_input_a_on_enter();
void state_input_b_on_enter();
void state_valhalla_preamp_bypass_on_enter();
void state_valhalla_preamp_engage_on_enter();
void state_output_a_on_enter();
void state_output_b_on_enter();
void state_output_c_on_enter();

//fsm states
//define states with FunctionState state(&func_on_enter_state, &func_in_state, &func_on_exit_state)
//you can use nullptr where no function is required

FunctionState state_input_a(&state_input_a_on_enter, nullptr, nullptr);
FunctionState state_input_b(&state_input_b_on_enter, nullptr, nullptr);
FunctionState state_valhalla_preamp_bypass(&state_valhalla_preamp_bypass_on_enter, nullptr, nullptr);
FunctionState state_valhalla_preamp_engage(&state_valhalla_preamp_engage_on_enter, nullptr, nullptr);
FunctionState state_output_a(&state_output_a_on_enter, nullptr, nullptr);
FunctionState state_output_b(&state_output_b_on_enter, nullptr, nullptr);
FunctionState state_output_c(&state_output_c_on_enter, nullptr, nullptr);
//You can have as many states as you'd like

//fsm
//define the fsm with the state it will start in
FunctionFsm fsm_input(&state_input_a);
FunctionFsm fsm_valhalla(&state_valhalla_preamp_bypass);
FunctionFsm fsm_output(&state_input_a);
//You can run as many state machines as you'd like

void trigger(Trigger event) {
    fsm_input.trigger(event);
    fsm_valhalla.trigger(event);
    fsm_output.trigger(event);
}

//fsm state functions
void state_input_a_on_enter() {
  digitalWrite(RELAY_INPUT, LOW);
}

void state_input_b_on_enter() {
  digitalWrite(RELAY_INPUT, HIGH);
}

void state_valhalla_preamp_bypass_on_enter() {
  digitalWrite(RELAY_VALHALLA, LOW);
}

void state_valhalla_preamp_engage_on_enter() {
  digitalWrite(RELAY_VALHALLA, HIGH);
}

void state_output_a_on_enter() {
  digitalWrite(RELAY_OUTPUT_A, LOW);
  digitalWrite(RELAY_OUTPUT_B, LOW);
}

void state_output_b_on_enter() {
  digitalWrite(RELAY_OUTPUT_A, HIGH);
  digitalWrite(RELAY_OUTPUT_B, LOW);
}

// this is valhalla out
void state_output_c_on_enter() {
  digitalWrite(RELAY_OUTPUT_A, HIGH);
  digitalWrite(RELAY_OUTPUT_B, HIGH);
  trigger(kTriggerValhallaBecameOutput);
}

void initfsm() {
//define transitions between states with
//myfsm.add_transition(&state_to_transition_from, &state_to_transition_to, &trigger, &func_on_transition
//you can use nullptr where no transition function is required

//it's useful to group all the add_transitions together so they can be easily called as needed

  fsm_input.add_transition(&state_input_a, &state_input_b, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_a, &state_input_b, kTriggerSelectInputB, nullptr);
  fsm_input.add_transition(&state_input_b, &state_input_a, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_b, &state_input_a, kTriggerSelectInputB, nullptr);

  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass, &state_valhalla_preamp_engage, kTriggerToggleValhallaPreamp, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass, &state_valhalla_preamp_engage, kTriggerSelectValhallaPreampEngage, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage, &state_valhalla_preamp_bypass, kTriggerToggleValhallaPreamp, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage, &state_valhalla_preamp_bypass, kTriggerSelectValhallaPreampBypass, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage, &state_valhalla_preamp_bypass, kTriggerValhallaBecameOutput, nullptr);

  fsm_output.add_transition(&state_output_a, &state_output_b, kTriggerToggleOutput, nullptr);
  fsm_output.add_transition(&state_output_b, &state_output_a, kTriggerSelectOutputA, nullptr);
  fsm_output.add_transition(&state_output_c, &state_output_a, kTriggerSelectOutputA, nullptr);
  fsm_output.add_transition(&state_output_a, &state_output_b, kTriggerSelectOutputB, nullptr);
  fsm_output.add_transition(&state_output_b, &state_output_b, kTriggerToggleOutput, nullptr);
  fsm_output.add_transition(&state_output_c, &state_output_b, kTriggerSelectOutputB, nullptr);
  fsm_output.add_transition(&state_output_a, &state_output_c, kTriggerSelectOutputC, nullptr);
  fsm_output.add_transition(&state_output_b, &state_output_c, kTriggerSelectOutputC, nullptr);
  fsm_output.add_transition(&state_output_c, &state_output_a, kTriggerToggleOutput, nullptr);
}

