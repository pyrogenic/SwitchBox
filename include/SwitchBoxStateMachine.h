#pragma once

// https://github.com/kekyo/gcc-toolchain/issues/3#issuecomment-356762548
#undef min
#undef max
#undef LITTLE_ENDIAN

#include <map>
#include <string>

#include "Fsm.h"
#include "Menu.h"

#define TRIGGER(FSM_NAME, ITEM_NAME) kTrigger_##FSM_NAME##_##ITEM_NAME

#define CYCLE_FSM(FSM_NAME, BEGIN_AT) TRIGGER(FSM_NAME, end), TRIGGER(FSM_NAME, prev), TRIGGER(FSM_NAME, next), TRIGGER(FSM_NAME, begin) = TRIGGER(FSM_NAME, BEGIN_AT)

#define TOGGLE_FSM(FSM_NAME) TRIGGER(FSM_NAME, toggle), TRIGGER(FSM_NAME, bypass), TRIGGER(FSM_NAME, engage)

enum Trigger {
  kTriggerNone = 0,

  TRIGGER(mode, day),
  TRIGGER(mode, night),
  TRIGGER(mode, off),
  CYCLE_FSM(mode, day),

  TRIGGER(input, digital),
  TRIGGER(input, analog),
  TRIGGER(input, aux),
  CYCLE_FSM(input, digital),

  TRIGGER(output, geshelli),
  TRIGGER(output, monolith),
  TRIGGER(output, valhalla),
  TRIGGER(output, speakers),
  TRIGGER(output, adc),
  CYCLE_FSM(output, geshelli),

  TOGGLE_FSM(loki),
  TOGGLE_FSM(bellari),
  TOGGLE_FSM(valhalla),
  TRIGGER(valhalla, force),
  TRIGGER(valhalla, release),
  TOGGLE_FSM(subwoofer),
  TOGGLE_FSM(level),
  TOGGLE_FSM(mute),

  TRIGGER(menu, none),
  TRIGGER(menu, quick),
  TRIGGER(menu, input),
  TRIGGER(menu, preamp),
  TRIGGER(menu, output),
  TRIGGER(menu, mode),
  CYCLE_FSM(menu, none),
};

Trigger operator++(Trigger t);

#undef TOGGLE_FSM
#undef CYCLE_FSM

extern std::map<Trigger, const char *> triggerNames;

const char *sbsm_trigger_name(Trigger event);
const char *sbsm_input_label();
const char *sbsm_subwoofer_label();
const char *sbsm_output_label();

void sbsm_setup();
void sbsm_loop();
void sbsm_trigger(Trigger state);

typedef class Fsm<Trigger> Sbsm;

class TriggerMenu : public Menu {
public:
  TriggerMenu(Trigger event, const char *name, Fsm<Trigger> *fsm, State *state) : Menu(name), m_trigger(event), m_fsm(fsm), m_state(state) {}
  void onEnter() override;
  bool isChecked() override;

private:
  Trigger m_trigger;
  Fsm<Trigger> *m_fsm;
  State *m_state;
};
