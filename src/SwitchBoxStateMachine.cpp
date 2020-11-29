#include "SwitchBoxStateMachine.h"

#include <Arduino.h>

#include "ABO.h"
#include "DebugLine.h"
#include "SwitchBox.h"

// This is how you avoid defining template bodies in the header
// https://stackoverflow.com/a/36825508
#include "Fsm.cpp"
typedef class Fsm<Trigger> Sbsm;
std::vector<Sbsm *> stateMachines;

#define SBSM(FSM_NAME, STATE_NAME)                                                           \
  Sbsm fsm_##FSM_NAME(#FSM_NAME, state_##FSM_NAME##_##STATE_NAME);                           \
  PROGMEM const char *FSM_NAME##_label() { return fsm_##FSM_NAME.get_current_state().name; } \
  void FSM_NAME##_setup()

#define STATE_ID(FSM_NAME, STATE_NAME) state_##FSM_NAME##_##STATE_NAME

#define STATE3(FSM_NAME, STATE_NAME, ENTER, IN, EXIT)                                                                                                           \
  void state_##FSM_NAME##_##STATE_NAME##_on_enter() ENTER void state_##FSM_NAME##_##STATE_NAME##_on_state() IN void state_##FSM_NAME##_##STATE_NAME##_on_exit() \
      EXIT State state_##FSM_NAME##_##STATE_NAME(#STATE_NAME, state_##FSM_NAME##_##STATE_NAME##_on_enter, state_##FSM_NAME##_##STATE_NAME##_on_state, state_##FSM_NAME##_##STATE_NAME##_on_exit)

#define STATE2(FSM_NAME, STATE_NAME, ENTER, EXIT) STATE3(FSM_NAME, STATE_NAME, ENTER, {}, EXIT)
#define STATE1(FSM_NAME, STATE_NAME, ENTER) STATE3(FSM_NAME, STATE_NAME, ENTER, {}, {})

#define STATE(FSM_NAME, STATE_NAME) STATE3(FSM_NAME, STATE_NAME, ;, ;, ;)

#define TRANSITION(FSM_NAME, EVENT, A, B) fsm_##FSM_NAME.add_transition(state_##FSM_NAME##_##A, state_##FSM_NAME##_##B, EVENT, nullptr)

#define TOGGLE(FSM_NAME, EVENT, A, B) \
  TRANSITION(FSM_NAME, EVENT, A, B);  \
  TRANSITION(FSM_NAME, EVENT, B, A)

#define TOGGLE_FSM(FSM_NAME, ENGAGE, BYPASS)                            \
  STATE1(FSM_NAME, bypass, BYPASS);                                     \
  STATE1(FSM_NAME, engage, ENGAGE);                                     \
  SBSM(FSM_NAME, bypass) {                                              \
    TOGGLE(FSM_NAME, kTrigger_##FSM_NAME##_toggle, bypass, engage);     \
    TRANSITION(FSM_NAME, kTrigger_##FSM_NAME##_engage, bypass, engage); \
    TRANSITION(FSM_NAME, kTrigger_##FSM_NAME##_bypass, engage, bypass); \
  }

#define TOGGLE_FSM_PIN(FSM_NAME)                                                      \
  TOGGLE_FSM(                                                                         \
      FSM_NAME,                                                                       \
      {                                                                               \
        abo_digitalWrite({kABIPinShiftRegister, kSout_engage_##FSM_NAME}, SOUT_HIGH); \
      },                                                                              \
      {                                                                               \
        abo_digitalWrite({kABIPinShiftRegister, kSout_engage_##FSM_NAME}, SOUT_LOW);  \
      })

STATE(input, digital);
STATE(input, analog);
STATE(input, aux);
SBSM(input, digital) {
  TRANSITION(input, kTrigger_input_next, digital, analog);
}

STATE(mode, day);
STATE(mode, night);
STATE(mode, off);
Sbsm fsm_mode("mode", state_mode_day);

// State       OutputA  OutputB  Headphones  Trigger
// Geshelli    low      (low)    low         -
// Valhalla    high     low      high        <engage valhalla>    // will bypass attenuator
// Speakers    high     high     (low)
// ADC         high     low      (low)
STATE(output, geshelli);
STATE(output, valhalla);
STATE(output, speakers);
STATE(output, adc);
Sbsm fsm_output("output", state_output_geshelli);

TOGGLE_FSM_PIN(loki);
TOGGLE_FSM_PIN(bellari);
TOGGLE_FSM(valhalla, {/* engage */}, {/* bypass */});
TOGGLE_FSM(subwoofer, {/* engage */}, {/* bypass */});
TOGGLE_FSM_PIN(level);
TOGGLE_FSM_PIN(mute);

std::map<Trigger, const char *> triggerNames;

const char *missingString = "MISSING";

const char *sbsm_trigger_name(Trigger event) {
  const auto it = triggerNames.find(event);
  if (it == triggerNames.end()) {
    return missingString;
  }
  return it->second;
}

void sbsm_trigger(Trigger event) {
  Serial_printf("\nsbsm_trigger: [%d] (%s)\n", event, sbsm_trigger_name(event));
  for (auto fsm : stateMachines) {
    fsm->trigger(event);
  }
}

typedef struct MenuDef {
  Trigger trigger;
  const char *label;
};

MenuDef menuDefs[] = {
    {kTrigger_input_next, "Next Input"},
};

void sbsm_setup() {
  for (const auto e : menuDefs) {
    triggerNames.insert({e.trigger, e.label});
  }
}

void sbsm_loop() {
  for (auto machine : stateMachines) {
    machine->run_machine();
  }
}
