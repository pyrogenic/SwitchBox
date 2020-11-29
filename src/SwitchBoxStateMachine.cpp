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

#define FSM(FSM_NAME) fsm_##FSM_NAME

#define SBSM(FSM_NAME, STATE_NAME)                                                         \
  Sbsm FSM(FSM_NAME)(#FSM_NAME, state_##FSM_NAME##_##STATE_NAME);                          \
  const char *sbsm_##FSM_NAME##_label() { return FSM(FSM_NAME).get_current_state().name; } \
  void FSM_NAME##_setup()

#define STATE_ID(FSM_NAME, STATE_NAME) state_##FSM_NAME##_##STATE_NAME

#define STATE3(FSM_NAME, STATE_NAME, ENTER, IN, EXIT)                                                                                                           \
  void state_##FSM_NAME##_##STATE_NAME##_on_enter() ENTER void state_##FSM_NAME##_##STATE_NAME##_on_state() IN void state_##FSM_NAME##_##STATE_NAME##_on_exit() \
      EXIT State state_##FSM_NAME##_##STATE_NAME(#STATE_NAME, state_##FSM_NAME##_##STATE_NAME##_on_enter, state_##FSM_NAME##_##STATE_NAME##_on_state, state_##FSM_NAME##_##STATE_NAME##_on_exit)

#define STATE2(FSM_NAME, STATE_NAME, ENTER, EXIT) STATE3(FSM_NAME, STATE_NAME, ENTER, {}, EXIT)
#define STATE1(FSM_NAME, STATE_NAME, ENTER) STATE3(FSM_NAME, STATE_NAME, ENTER, {}, {})

#define STATE(FSM_NAME, STATE_NAME) STATE3(FSM_NAME, STATE_NAME, ;, ;, ;)

#define TRANSITION(FSM_NAME, EVENT, A, B) FSM(FSM_NAME).add_transition(state_##FSM_NAME##_##A, state_##FSM_NAME##_##B, EVENT, nullptr)

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

#define CYCLE_FSM_STATES(FSM_NAME)                                                                   \
  constexpr size_t FSM_NAME##_STATE_COUNT = kTrigger_##FSM_NAME##_end - kTrigger_##FSM_NAME##_begin; \
  StateW FSM_NAME##_states[FSM_NAME##_STATE_COUNT] =

#define CYCLE_FSM(FSM_NAME)                                                                \
  Sbsm FSM(FSM_NAME)(#FSM_NAME, FSM_NAME##_states[0].state);                               \
  const char *sbsm_##FSM_NAME##_label() { return FSM(FSM_NAME).get_current_state().name; } \
  void FSM_NAME##_setup() {                                                                \
    /* next / prev */                                                                      \
    for (int i = 0; i < FSM_NAME##_STATE_COUNT; ++i) {                                     \
      State &a = FSM_NAME##_states[i].state;                                               \
      State &b = FSM_NAME##_states[(i + 1) % FSM_NAME##_STATE_COUNT].state;                \
      FSM(FSM_NAME).add_transition(a, b, TRIGGER(FSM_NAME, next), nullptr);                \
      FSM(FSM_NAME).add_transition(b, a, TRIGGER(FSM_NAME, prev), nullptr);                \
    }                                                                                      \
    /* for each destination, add a transition from each other state directly to it */      \
    for (Trigger i = kTrigger_##FSM_NAME##_begin; i < kTrigger_##FSM_NAME##_end; ++i) {    \
      State &a = FSM_NAME##_states[i - kTrigger_##FSM_NAME##_begin].state;                 \
      for (int s = 0; s < FSM_NAME##_STATE_COUNT; ++s) {                                   \
        State &b = FSM_NAME##_states[(i + 1) % FSM_NAME##_STATE_COUNT].state;              \
        if (&a != &b) {                                                                    \
          FSM(FSM_NAME).add_transition(b, a, i, nullptr);                                  \
        }                                                                                  \
      }                                                                                    \
    }                                                                                      \
  }

STATE(input, digital);
STATE(input, analog);
STATE(input, aux);
SBSM(input, digital) {
  TRANSITION(input, kTrigger_input_next, digital, analog);
}

STATE(mode, day);
STATE(mode, night);
STATE(mode, off);
SBSM(mode, day);

// State       OutputA  OutputB  Headphones  Trigger
// Geshelli    low      (low)    low         -
// Valhalla    high     high     high        <engage valhalla>    // will bypass attenuator
// Speakers    high     low      (low)
// ADC         high     high     (low)
STATE1(output, geshelli, {
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_a}, SOUT_LOW);
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_b}, SOUT_LOW);
  abo_digitalWrite({kABIPinShiftRegister, kSout_headphones}, SOUT_LOW);
});
STATE2(
    output, valhalla,
    {
      abo_digitalWrite({kABIPinShiftRegister, kSout_output_a}, SOUT_HIGH);
      abo_digitalWrite({kABIPinShiftRegister, kSout_output_b}, SOUT_HIGH);
      abo_digitalWrite({kABIPinShiftRegister, kSout_headphones}, SOUT_HIGH);
      sbsm_trigger(kTrigger_valhalla_force);
    },
    { sbsm_trigger(kTrigger_valhalla_release); });
STATE1(output, speakers, {
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_a}, SOUT_HIGH);
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_b}, SOUT_LOW);
});
STATE1(output, adc, {
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_a}, SOUT_HIGH);
  abo_digitalWrite({kABIPinShiftRegister, kSout_output_b}, SOUT_HIGH);
});

struct StateW {
  State &state;
};

CYCLE_FSM_STATES(output){{state_output_geshelli}, {state_output_valhalla}, {state_output_speakers}, {state_output_adc}};
CYCLE_FSM(output);

TOGGLE_FSM_PIN(loki);
TOGGLE_FSM_PIN(bellari);
STATE1(valhalla, bypass, { abo_digitalWrite({kABIPinShiftRegister, kSout_engage_valhalla}, SOUT_LOW); });
STATE1(valhalla, engage, { abo_digitalWrite({kABIPinShiftRegister, kSout_engage_valhalla}, SOUT_HIGH); });
STATE1(valhalla, bypass_force_engage, { state_valhalla_engage_on_enter(); });
STATE1(valhalla, engage_force_engage, { state_valhalla_engage_on_enter(); });
SBSM(valhalla, bypass) {
  TOGGLE(valhalla, kTrigger_valhalla_toggle, bypass, engage);
  TOGGLE(valhalla, kTrigger_valhalla_toggle, bypass_force_engage, engage_force_engage);
  TRANSITION(valhalla, kTrigger_valhalla_force, bypass, bypass_force_engage);
  TRANSITION(valhalla, kTrigger_valhalla_force, engage, engage_force_engage);
  TRANSITION(valhalla, kTrigger_valhalla_release, bypass_force_engage, bypass);
  TRANSITION(valhalla, kTrigger_valhalla_release, engage_force_engage, engage);
};
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
