#include "Menu.h"

// class Menu {
// public:
//   Menu() : m_back(*this) {}

//   template <class TDisplay, class TTiler, typename TColor>
//   void populate(PicoMenu<TDisplay, TTiler, TColor> &picoMenu);

//   Menu &enter();
//   Menu &back();
//   void up();
//   void down();
// private:
//   Menu &m_back;
//   std::vector<Menu> m_items;
// };

//   void populate(PicoMenu<TDisplay, TTiler, TColor> &picoMenu);
template <class TDisplay, class TTiler, typename TColor> void Menu::populate(PicoMenu<TDisplay, TTiler, TColor> &picoMenu) {
  // picoMenu.add
  PicoMenuItem<TDisplay, TTiler, TColor> *p = picoMenu.getNext();
  for (auto item : this->m_items.begin()) {
    if (!p) {
      p = new PicoMenuItem<TDisplay, TTiler, TColor>(item->name());
      picoMenu.add(p);
    } else {
      p->m_name = item->name();
    }
    p = picoMenu.getNext(p);
  }
  while (p) {
    auto next = picoMenu.getNext(p);
    picoMenu.remove(p);
    delete p;
    p = next;
  }
}

void Menu::add(Menu *menu) {
  if (menu) {
    Serial_printf("%s: adding new menu item %s\n", m_name, menu->m_name);
    m_items.push_back(menu);
    Serial_printf("%s: added new menu item %s\n", m_name, (*m_items.rbegin())->m_name);
  }
}
