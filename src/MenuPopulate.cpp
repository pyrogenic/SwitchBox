#include "Menu.h"

template <class TDisplay, class TTiler, typename TColor> void Menu::populate(PicoMenu<TDisplay, TTiler, TColor> &picoMenu, CSS<TColor> &menuItemStyle) {
  // picoMenu.add
  PicoMenuItem<TDisplay, TTiler, TColor> *p = picoMenu.getNext();
  for (auto item : m_items) {
    if (!p) {
      p = new PicoMenuItem<TDisplay, TTiler, TColor>();
      picoMenu.add(*p);
    }
    p->setName(item->name());
    p->setCss(&menuItemStyle);
    p = picoMenu.getNext(p);
  }
  while (p) {
    auto next = picoMenu.getNext(p);
    picoMenu.remove(*p);
    delete p;
    p = next;
  }
}