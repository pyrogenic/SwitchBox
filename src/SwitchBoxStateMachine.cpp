#include "SwitchBoxStateMachine.h"
#include "SwitchBox.h"
#include <Arduino.h>

void state_input_digital_on_enter();
void state_input_analog_on_enter();
void state_subwoofer_bypass_on_enter();
void state_subwoofer_engage_on_enter();
void state_output_geshelli_on_enter();
void state_output_geshelli_on_enter();
void state_output_valhalla_on_enter();
void state_output_valhalla_on_exit();
void state_output_monitor_on_enter();
void state_output_monitor_on_enter();
void state_output_monitor_on_exit();

// fsm states
// define states with FunctionState state(&func_on_enter_state, &func_in_state,
// &func_on_exit_state) you can use nullptr where no function is required

FunctionState state_input_digital(&state_input_digital_on_enter, nullptr, nullptr);
FunctionState state_input_analog(&state_input_analog_on_enter, nullptr, nullptr);
FunctionState state_subwoofer_bypass(&state_subwoofer_bypass_on_enter, nullptr, nullptr);
FunctionState state_subwoofer_engage(&state_subwoofer_engage_on_enter, nullptr, nullptr);
FunctionState state_subwoofer_bypass_locked(&state_subwoofer_bypass_on_enter, nullptr, nullptr);
FunctionState state_subwoofer_engage_locked(&state_subwoofer_bypass_on_enter, nullptr, nullptr);
FunctionState state_output_geshelli(&state_output_geshelli_on_enter, nullptr, nullptr);
FunctionState state_output_valhalla(&state_output_valhalla_on_enter, nullptr, &state_output_valhalla_on_exit);
FunctionState state_output_monitor(&state_output_monitor_on_enter, nullptr, nullptr);

// define the fsm with the state it will start in
FunctionFsm fsm_input(&state_input_digital);
FunctionFsm fsm_output(&state_output_geshelli);
// this has to start locked b/c default output isn't Valhala
FunctionFsm fsm_subwoofer(&state_subwoofer_engage_locked);

std::map<FunctionFsm *, const std::string> stateMachineNames;
std::map<FunctionState *, const std::string> stateNames;
std::map<Trigger, const std::string> triggerNames;

const std::string missingString = "MISSING";

const std::string &sbsm_trigger_name(const Trigger event) {
  const auto it = triggerNames.find(event);
  if (it == triggerNames.end()) {
    return missingString;
  }
  return it->second;
}

const std::string &sbsm_input_label() {
  const auto it = stateNames.find(&fsm_input.get_current_state());
  if (it == stateNames.end()) {
    return missingString;
  }
  return it->second;
}

const std::string &sbsm_subwoofer_label() {
  const auto it = stateNames.find(&fsm_subwoofer.get_current_state());
  if (it == stateNames.end()) {
    return missingString;
  }
  return it->second;
}

const std::string &sbsm_output_label() {
  const auto it = stateNames.find(&fsm_output.get_current_state());
  if (it == stateNames.end()) {
    return missingString;
  }
  return it->second;
}

void sbsm_trigger(Trigger event) {
  Serial.printf("\nsbsm_trigger: [%d] (%s)\n", event, sbsm_trigger_name(event).c_str());
  for (auto fsm : stateMachineNames) {
    fsm.first->trigger(event);
  }
}

// fsm state functions
void state_input_digital_on_enter() {
  digitalWrite(RELAY_INPUT, LOW);
}

void state_input_analog_on_enter() {
  digitalWrite(RELAY_INPUT, HIGH);
}

void state_subwoofer_bypass_on_enter() {
  digitalWrite(RELAY_SUB, LOW);
}

void state_subwoofer_engage_on_enter() {
  digitalWrite(RELAY_SUB, HIGH);
}

void state_output_geshelli_on_enter() {
  digitalWrite(RELAY_AMP, LOW);
  digitalWrite(RELAY_MONITOR, LOW);
}

void state_output_valhalla_on_enter() {
  digitalWrite(RELAY_AMP, HIGH);
  digitalWrite(RELAY_MONITOR, LOW);
  sbsm_trigger(kTriggerPreampEngaged);
}

void state_output_valhalla_on_exit() {
  sbsm_trigger(kTriggerPreampBypassed);
}

void state_output_monitor_on_enter() {
  digitalWrite(RELAY_AMP, LOW);
  digitalWrite(RELAY_MONITOR, HIGH);
}

void sbsm_setup() {
  stateMachineNames.emplace(&fsm_input, "Input");
  stateMachineNames.emplace(&fsm_subwoofer, "Sub");
  stateMachineNames.emplace(&fsm_output, "Output");

  stateNames.emplace(&state_input_digital, "Digital");
  stateNames.emplace(&state_input_analog, "Analog");
  stateNames.emplace(&state_subwoofer_bypass, "Sub Off");
  stateNames.emplace(&state_subwoofer_engage, "Sub On");
  stateNames.emplace(&state_subwoofer_bypass_locked, "(Sub Off)");
  stateNames.emplace(&state_subwoofer_engage_locked, "(Sub On)");
  stateNames.emplace(&state_output_geshelli, "Geshelli");
  stateNames.emplace(&state_output_valhalla, "Valhalla");
  stateNames.emplace(&state_output_monitor, "Monitors");

  triggerNames.emplace(kTriggerToggleInput, "Next Input");
  triggerNames.emplace(kTriggerSelectInputDigital, "Digital In");
  triggerNames.emplace(kTriggerSelectInputAnalog, "Analog In");

  triggerNames.emplace(kTriggerToggleSubwoofer, "Toggle Subwoofer");
  triggerNames.emplace(kTriggerSubwooferBypass, "Subwoofer Off");
  triggerNames.emplace(kTriggerSubwooferEngage, "Subwoofer On");

  triggerNames.emplace(kTriggerToggleOutput, "Next Output");
  triggerNames.emplace(kTriggerSelectOutputGeshelli, "Gesheli");
  triggerNames.emplace(kTriggerSelectOutputValhalla, "Valhalla");

  triggerNames.emplace(kTriggerActivateMonitor, "Monitor");

  triggerNames.emplace(kTriggerPreampEngaged, "Preamp Available");
  triggerNames.emplace(kTriggerPreampBypassed, "No Preamp");

  // define transitions between states with
  // myfsm.add_transition(&state_to_transition_from, &state_to_transition_to,
  // &trigger, &func_on_transition you can use nullptr where no transition
  // function is required

  // it's useful to group all the add_transitions together so they can be easily
  // called as needed

  fsm_input.add_transition(&state_input_digital, &state_input_analog, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_digital, &state_input_analog, kTriggerSelectInputAnalog, nullptr);
  fsm_input.add_transition(&state_input_analog, &state_input_digital, kTriggerToggleInput, nullptr);
  fsm_input.add_transition(&state_input_analog, &state_input_digital, kTriggerSelectInputDigital, nullptr);

  fsm_subwoofer.add_transition(&state_subwoofer_bypass, &state_subwoofer_engage, kTriggerToggleSubwoofer, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_bypass, &state_subwoofer_engage, kTriggerSubwooferEngage, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_bypass, &state_subwoofer_bypass_locked, kTriggerPreampBypassed, nullptr);

  fsm_subwoofer.add_transition(&state_subwoofer_engage, &state_subwoofer_bypass, kTriggerToggleSubwoofer, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_engage, &state_subwoofer_bypass, kTriggerSubwooferBypass, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_engage, &state_subwoofer_engage_locked, kTriggerPreampBypassed, nullptr);

  fsm_subwoofer.add_transition(&state_subwoofer_bypass_locked, &state_subwoofer_engage_locked, kTriggerToggleSubwoofer, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_bypass_locked, &state_subwoofer_engage_locked, kTriggerSubwooferEngage, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_bypass_locked, &state_subwoofer_bypass, kTriggerPreampEngaged, nullptr);

  fsm_subwoofer.add_transition(&state_subwoofer_engage_locked, &state_subwoofer_bypass_locked, kTriggerToggleSubwoofer, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_engage_locked, &state_subwoofer_bypass_locked, kTriggerSubwooferBypass, nullptr);
  fsm_subwoofer.add_transition(&state_subwoofer_engage_locked, &state_subwoofer_engage, kTriggerPreampEngaged, nullptr);

  fsm_output.add_transition(&state_output_geshelli, &state_output_valhalla, kTriggerToggleOutput, nullptr);
  fsm_output.add_transition(&state_output_geshelli, &state_output_valhalla, kTriggerSelectOutputValhalla, nullptr);
  fsm_output.add_transition(&state_output_geshelli, &state_output_monitor, kTriggerActivateMonitor, nullptr);

  fsm_output.add_transition(&state_output_valhalla, &state_output_geshelli, kTriggerToggleOutput, nullptr);
  fsm_output.add_transition(&state_output_valhalla, &state_output_geshelli, kTriggerSelectOutputGeshelli, nullptr);
  fsm_output.add_transition(&state_output_valhalla, &state_output_monitor, kTriggerActivateMonitor, nullptr);

  fsm_output.add_transition(&state_output_monitor, &state_output_geshelli, kTriggerToggleOutput, nullptr);
  fsm_output.add_transition(&state_output_monitor, &state_output_geshelli, kTriggerSelectOutputGeshelli, nullptr);
  fsm_output.add_transition(&state_output_monitor, &state_output_valhalla, kTriggerSelectOutputValhalla, nullptr);
}

void sbsm_loop() {
  // Serial.println();
  for (auto fsm : stateMachineNames) {
    FunctionFsm *machine = fsm.first;
    // const std::string &currentStateName = stateNames.at(&machine->get_current_state());
    // Serial.printf("\t%s: %s", fsm.second.c_str(), currentStateName.c_str());
    machine->run_machine();
  }
}