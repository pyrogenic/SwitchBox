#pragma once

// https://github.com/kekyo/gcc-toolchain/issues/3#issuecomment-356762548
#undef min
#undef max
#undef LITTLE_ENDIAN

#include <map>
#include <string>

#include "Fsm.h"

#define TRIGGER(FSM_NAME, ITEM_NAME) kTrigger_##FSM_NAME##_##ITEM_NAME
#define BEGIN_FSM(FSM_NAME) TRIGGER(FSM_NAME, begin)
#define END_FSM(FSM_NAME) TRIGGER(FSM_NAME, end)

#define CYCLE_FSM(FSM_NAME) BEGIN_FSM(FSM_NAME), TRIGGER(FSM_NAME, prev), TRIGGER(FSM_NAME, next)

#define TOGGLE_FSM(FSM_NAME) TRIGGER(FSM_NAME, toggle), TRIGGER(FSM_NAME, bypass), TRIGGER(FSM_NAME, engage), END_FSM(FSM_NAME)

enum Trigger {
  CYCLE_FSM(input),
  TRIGGER(input, digital),
  TRIGGER(input, analog),
  TRIGGER(input, aux),
  END_FSM(input),

  CYCLE_FSM(output),
  TRIGGER(output, digital),
  TRIGGER(output, analog),
  TRIGGER(output, aux),
  END_FSM(output),

  TOGGLE_FSM(loki),
  TOGGLE_FSM(bellari),
  TOGGLE_FSM(valhalla),
  TOGGLE_FSM(subwoofer),
  TOGGLE_FSM(level),
  TOGGLE_FSM(mute),

  CYCLE_FSM(menu),
  TRIGGER(menu, none),
  TRIGGER(menu, quick),
  TRIGGER(menu, input),
  TRIGGER(menu, preamp),
  TRIGGER(menu, output),
  TRIGGER(menu, mode),
  END_FSM(menu),
};

#undef TOGGLE_FSM

extern std::map<Trigger, const char *> triggerNames;

const char *sbsm_trigger_name(Trigger event);
const char *sbsm_input_label();
const char *sbsm_subwoofer_label();
const char *sbsm_output_label();

void sbsm_setup();
void sbsm_loop();
void sbsm_trigger(Trigger state);
