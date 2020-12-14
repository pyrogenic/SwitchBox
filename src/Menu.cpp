#include "Menu.h"

void Menu::add(Menu *menu) {
  if (menu) {
    Serial_printf("%s: adding new menu item %s\n", m_name, menu->m_name);
    m_items.push_back(menu);
    Serial_printf("%s: added new menu item %s\n", m_name, (*m_items.rbegin())->m_name);
  }
}
