#include "Menu.h"

template <class TDisplay, class TTiler, typename TColor> void Menu::populate(PicoMenu<TDisplay, TTiler, TColor> &picoMenu, CSS<TColor> &menuItemStyle) {
  PicoMenuItem<TDisplay, TTiler, TColor> *p = picoMenu.getNext();
  Serial_printf("%s: populate: %d items\n", m_name, m_items.size());
  for (auto item : m_items) {
    if (!p) {
      Serial_printf("  creating new menu item\n");
      p = new PicoMenuItem<TDisplay, TTiler, TColor>();
      Serial_printf("  new menu item: %08x\n", p);
      picoMenu.add(p);
    }
    p->setName(item->name());
    p->setCss(&menuItemStyle);
    Serial_printf("  set name: %s\n", p->name());
    p = picoMenu.getNext(p);
  }
  while (p) {
    auto next = picoMenu.getNext(p);
    Serial_printf("  removing extra item: %08x (%s)\n", p, p->name());
    picoMenu.remove(p);
    delete p;
    p = next;
  }
}