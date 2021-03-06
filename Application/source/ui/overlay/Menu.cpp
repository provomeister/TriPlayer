#include "ui/overlay/Menu.hpp"

// Padding on sides
#define X_PADDING 20
// Padding at top and bottom
#define Y_PADDING 10
// Width of background
#define WIDTH 380

namespace CustomOvl {
    Menu::Menu() : Overlay() {
        // Background (will be resized vertically to fit buttons)
        this->bg = new Aether::Rectangle(0, 0, WIDTH, 1, 25);
        this->addElement(this->bg);

        // Container which will hold buttons
        this->btns = new Aether::Container(this->bg->x(), this->bg->y(), this->bg->w(), 720);
        this->addElement(this->btns);
        this->setFocused(this->btns);

        this->nextY = Y_PADDING;
        this->topBtn = nullptr;
    }

    void Menu::addButton(CustomElm::MenuButton * b) {
        // Treat a nullptr as done and position everything
        if (b == nullptr) {
            this->bg->setRectSize(this->bg->w(), this->nextY + Y_PADDING);
            this->bg->setXY((this->w() - this->bg->w())/2, (this->h() -  this->bg->h())/2);
            this->btns->setXY(this->bg->x(), this->bg->y());
            this->btns->setH(this->nextY - this->btns->y());
            this->setTopLeft(this->bg->x(), this->bg->y());
            this->setBottomRight(this->bg->x() + this->bg->w(), this->bg->y() + this->bg->h());
            return;
        }

        // Otherwise position button underneath
        if (this->topBtn == nullptr) {
            this->topBtn = b;
        }
        b->setXY(this->btns->x() + X_PADDING, this->bg->y() + this->nextY);
        b->setW(this->btns->w() - 2*X_PADDING);
        this->btns->addElement(b);
        this->nextY += b->h();
    }

    void Menu::addSeparator(Aether::Colour c) {
        // Create line element and position
        Aether::Rectangle * r = new Aether::Rectangle(this->btns->x() + X_PADDING, this->bg->y() + this->nextY + 6, this->btns->w() - 2*X_PADDING, 1);
        r->setColour(c);
        this->btns->addElement(r);
        this->nextY += 12;
    }

    void Menu::setBackgroundColour(Aether::Colour c) {
        this->bg->setColour(c);
    }

    void Menu::resetHighlight() {
        if (this->topBtn != nullptr) {
            this->btns->setFocused(this->topBtn);
        }
    }
};