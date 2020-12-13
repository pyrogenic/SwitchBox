#pragma once

#include "DebugLine.h"
#include <lcdgfx.h>
#include <vector>

template <typename TColor> struct CSS {
  const uint8_t *font;
  int8_t marginLeft;
  int8_t marginTop;
  int8_t marginRight;
  int8_t marginBottom;
  int8_t padLeft;
  int8_t padTop;
  int8_t padRight;
  int8_t padBottom;
  TColor fg;
  TColor bg;
};

template <typename T> NanoRect borderRect(NanoRect rect, const CSS<T> &css) {
  rect.p1.x += css.marginLeft;
  rect.p1.y += css.marginTop;
  rect.p2.x -= css.marginRight;
  rect.p2.y -= css.marginBottom;
  return rect;
}

template <typename T> NanoRect contentRect(NanoRect rect, const CSS<T> &css) {
  rect.p1.x += css.marginLeft + css.padLeft;
  rect.p1.y += css.marginTop + css.padTop;
  rect.p2.x -= css.marginRight + css.padRight;
  rect.p2.y -= css.marginBottom + css.padBottom;
  return rect;
}

/**
 * Class implements menu, organized as the list.
 * Each item may have different width
 */
template <class TDisplay, class TTiler, typename TColor> class PicoMenu : public NanoMenu<TTiler> {
public:
  using NanoMenu<TTiler>::NanoMenu;
  PicoMenu(CSS<TColor> css) : NanoMenu<TTiler>(), m_css(css), m_dirty(true) {}

  void update() override {
    if (m_dirty) {
      updateMenuItemsPosition();
    }
    NanoMenu<TTiler>::update();
  }

  /**
   * Draw all menu items in NanoEngine buffer
   */
  void draw() override {
    auto canvas = NanoMenu<TTiler>::getTiler().getCanvas();
    canvas.setColor(this->m_css.fg);
    canvas.drawRect(borderRect(this->m_rect, m_css));
    NanoMenu<TTiler>::draw();
  }

protected:
  void updateMenuItemsPosition() override {
    m_dirty = false;
    auto rect = contentRect(this->m_rect, m_css);
    lcdint_t x = rect.p1.x;
    lcdint_t y = rect.p1.y;
    lcdint_t width = rect.width();
    NanoObject<TTiler> *p = NanoMenu<TTiler>::getNext();
    while (p) {
      Serial_printf(" x: %d y: %d\n", x, y);
      p->setPos({x, y});
      lcdint_t height = p->height();
      p->setSize({width, height});
      if (height <= 0) {
        m_dirty = true;
      } else {
        y += height;
      }
      Serial_printf(" height: %d dirty: %s\n", height, m_dirty ? "true" : "false");
      p = this->getNext(p);
    }
  }

private:
  CSS<TColor> m_css;
  bool m_dirty;
};

/**
 * Template class for font menu item with user-defined font
 */
template <class TDisplay, class TTiler, typename TColor> class PicoMenuItem : public NanoMenuItem<TTiler> {
public:
  typedef NanoMenuItem<TTiler> This;
  /**
   * Creates instance of test menu item
   *
   * @param name text of the item to display
   */
  PicoMenuItem(const char *name, CSS<TColor> css) : NanoMenuItem<TTiler>({0, 0}), m_name(name), m_css(css) {
    This::setSize({
        0,
        0,
    });
  }

  /**
   * Updates menu item state. Automatically resizes menu item if width is
   * not defined yet
   */
  void update() override {
    if (rect().height() <= 1) {
      if (getDisplay()) {
        Serial_printf("PicoMenuItem.update: %s\n", m_name);
        auto font = getDisplay()->getFont();
        if (!font) {
          Serial_printf("  No font selected!\n", m_name);
          return;
        }
        lcduint_t height;
        lcdint_t width = font->getTextSize(m_name, &height);
        lcdint_t paddedWidth = width + m_css.marginLeft + m_css.marginRight + m_css.padLeft + m_css.padRight;
        lcdint_t paddedHeight = height + m_css.marginTop + m_css.marginBottom + m_css.padTop + m_css.padBottom;
        Serial_printf(" -- width/padded: %d/%d height/padded: %d/%d\n", width, paddedWidth, height, paddedHeight);
        This::resize({
            paddedWidth,
            paddedHeight,
        });
        //   } else {
        //     // At this point we don't know font to be used by a user
        //     setSize({width(), (lcduint_t)8});
      }
    }
  }

  /**
   * Draws text menu item in the NanoEngine buffer
   */
  void draw() override {
    auto canvas = this->getTiler().getCanvas();
    if (this->isFocused()) {
      canvas.setMode(CANVAS_MODE_TRANSPARENT);
      canvas.setColor(this->m_css.fg);
      canvas.fillRect(borderRect(rect(), css()));
      canvas.setColor(this->m_css.bg);
    } else {
      canvas.setMode(CANVAS_MODE_BASIC);
      canvas.setColor(this->m_css.fg);
    }
    canvas.printFixed(textLeft(), textTop(), m_name);
  }

  const NanoRect &rect() { return this->m_rect; }
  lcdint_t textLeft() { return rect().p1.x + m_css.marginLeft + m_css.padLeft; }
  lcdint_t textTop() { return rect().p1.y + m_css.marginTop + m_css.padTop; }
  const CSS<TColor> &css() { return m_css; }

protected:
  /** Menu item text */
  const char *m_name;
  CSS<TColor> m_css;

private:
  TDisplay *getDisplay() {
    if (!This::hasTiler()) {
      return nullptr;
    }
    TDisplay &display = This::getTiler().getDisplay();
    display.setFixedFont(m_css.font);
    return &display;
  }
};

class Menu {
public:
  Menu() : back(*this) {}

private:
  Menu &back;
  std::vector<Menu> items;
};
