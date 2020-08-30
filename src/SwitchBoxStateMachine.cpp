#include "SwitchBoxStateMachine.h"
#include "SwitchBox.h"
#include <Arduino.h>

SwitchBoxState publicState = {};

void state_input_a_on_state();
void state_input_b_on_state();
void state_valhalla_preamp_bypass_on_state();
void state_valhalla_preamp_engage_on_enter();
void state_valhalla_preamp_engage_on_state();
void state_output_a_on_state();
void state_output_b_on_state();
void state_output_c_on_enter();
void state_output_c_on_state();

// fsm states
// define states with FunctionState state(&func_on_state_state, &func_in_state,
// &func_on_exit_state) you can use nullptr where no function is required

FunctionState state_input_a(nullptr, &state_input_a_on_state, nullptr);
FunctionState state_input_b(nullptr, &state_input_b_on_state, nullptr);
FunctionState state_valhalla_preamp_bypass(nullptr, &state_valhalla_preamp_bypass_on_state, nullptr);
FunctionState state_valhalla_preamp_engage(nullptr, &state_valhalla_preamp_engage_on_state, nullptr);
FunctionState state_valhalla_preamp_bypass_locked(nullptr, &state_valhalla_preamp_bypass_on_state, nullptr);
FunctionState state_valhalla_preamp_engage_locked(nullptr, &state_valhalla_preamp_bypass_on_state, nullptr);
FunctionState state_output_a(nullptr, &state_output_a_on_state, nullptr);
FunctionState state_output_b(nullptr, &state_output_b_on_state, nullptr);
FunctionState state_output_c(&state_output_c_on_enter, &state_output_c_on_state, nullptr);

std::map<FunctionFsm *, const std::string> stateMachineNames;
std::map<FunctionState *, const std::string> stateNames;
std::map<Trigger, const std::string> triggerNames;

// fsm
// define the fsm with the state it will start in
FunctionFsm fsm_input(&state_input_a);
FunctionFsm fsm_valhalla(&state_valhalla_preamp_bypass);
FunctionFsm fsm_output(&state_input_a);
// You can run as many state machines as you'd like

void sbsm_trigger(Trigger event) {
  fsm_input.trigger(event);
  fsm_valhalla.trigger(event);
  fsm_output.trigger(event);
}

// fsm state functions
void state_input_a_on_state() { digitalWrite(RELAY_INPUT, LOW); }

void state_input_b_on_state() { digitalWrite(RELAY_INPUT, HIGH); }

void state_valhalla_preamp_bypass_on_state() { digitalWrite(RELAY_VALHALLA, LOW); }

void state_valhalla_preamp_engage_on_state() { digitalWrite(RELAY_VALHALLA, HIGH); }

void state_output_a_on_state() {
  digitalWrite(RELAY_OUTPUT_A, LOW);
  digitalWrite(RELAY_OUTPUT_B, LOW);
}

void state_output_b_on_state() {
  digitalWrite(RELAY_OUTPUT_A, HIGH);
  digitalWrite(RELAY_OUTPUT_B, LOW);
}

// this is valhalla out
void state_output_c_on_enter() { sbsm_trigger(kTriggerValhallaOutputLocked); }

void state_output_c_on_state() {
  digitalWrite(RELAY_OUTPUT_A, HIGH);
  digitalWrite(RELAY_OUTPUT_B, HIGH);
}

void state_output_c_on_exit() { sbsm_trigger(kTriggerValhallaOutputUnlocked); }

void sbsm_setup() {
  stateMachineNames.emplace(&fsm_input, "Input");
  stateMachineNames.emplace(&fsm_valhalla, "Preamp");
  stateMachineNames.emplace(&fsm_output, "Output");

  stateNames.emplace(&state_input_a, "Digital");
  stateNames.emplace(&state_input_b, "Analog");
  stateNames.emplace(&state_valhalla_preamp_bypass, "Bypass");
  stateNames.emplace(&state_valhalla_preamp_engage, "Engage");
  stateNames.emplace(&state_valhalla_preamp_bypass_locked, "(Bypass)");
  stateNames.emplace(&state_valhalla_preamp_engage_locked, "(Engage)");
  stateNames.emplace(&state_output_a, "Speakers");
  stateNames.emplace(&state_output_b, "Geshelli");
  stateNames.emplace(&state_output_c, "Valhalla");

  triggerNames.emplace(kTriggerToggleInput, "Next Input");
  triggerNames.emplace(kTriggerSelectInputA, "Digital In");
  triggerNames.emplace(kTriggerSelectInputB, "Analog In");

  triggerNames.emplace(kTriggerToggleValhallaPreamp, "Toggle Preamp");
  triggerNames.emplace(kTriggerSelectValhallaPreampBypass, "Bypass Preamp");
  triggerNames.emplace(kTriggerSelectValhallaPreampEngage, "Engage Preamp");

  triggerNames.emplace(kTriggerToggleOutput, "Next Output");
  triggerNames.emplace(kTriggerSelectOutputA, "Speakers Out");
  triggerNames.emplace(kTriggerSelectOutputB, "Geshelli Out");
  triggerNames.emplace(kTriggerSelectOutputC, "Valhalla Out");

  triggerNames.emplace(kTriggerValhallaOutputLocked, "ValhallaOutputLocked");
  triggerNames.emplace(kTriggerValhallaOutputUnlocked, "ValhallaOutputUnlocked");
  // define transitions between states with
  // myfsm.add_transition(&state_to_transition_from, &state_to_transition_to,
  // &trigger, &func_on_transition you can use nullptr where no transition
  // function is required

  // it's useful to group all the add_transitions together so they can be easily
  // called as needed

  fsm_input.add_transition(&state_input_a, &state_input_b, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_a, &state_input_b, kTriggerSelectInputB, nullptr);
  fsm_input.add_transition(&state_input_b, &state_input_a, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_b, &state_input_a, kTriggerSelectInputB, nullptr);

  // valhalla toggle / toggle while locked
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass, &state_valhalla_preamp_engage, kTriggerToggleValhallaPreamp, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage, &state_valhalla_preamp_bypass, kTriggerToggleValhallaPreamp, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass_locked, &state_valhalla_preamp_engage_locked, kTriggerToggleValhallaPreamp, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage_locked, &state_valhalla_preamp_bypass_locked, kTriggerToggleValhallaPreamp, nullptr);

  // valhalla engage / engage while locked
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass, &state_valhalla_preamp_engage, kTriggerSelectValhallaPreampEngage, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass_locked, &state_valhalla_preamp_engage_locked, kTriggerSelectValhallaPreampEngage, nullptr);

  // valhalla bypass / bypass while locked
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage_locked, &state_valhalla_preamp_bypass_locked, kTriggerSelectValhallaPreampBypass, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage_locked, &state_valhalla_preamp_bypass_locked, kTriggerSelectValhallaPreampBypass, nullptr);

  // valhalla output lock
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass, &state_valhalla_preamp_bypass_locked, kTriggerValhallaOutputLocked, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage, &state_valhalla_preamp_engage_locked, kTriggerValhallaOutputLocked, nullptr);

  // valhalla output unlock
  fsm_valhalla.add_transition(&state_valhalla_preamp_bypass_locked, &state_valhalla_preamp_bypass, kTriggerValhallaOutputUnlocked, nullptr);
  fsm_valhalla.add_transition(&state_valhalla_preamp_engage_locked, &state_valhalla_preamp_engage, kTriggerValhallaOutputUnlocked, nullptr);

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

void sbsm_loop() {
  for (auto fsm : stateMachineNames) {
    fsm.first->run_machine();
  }
}