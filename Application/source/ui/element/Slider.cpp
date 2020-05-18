#include "Slider.hpp"

// Amount of pixels either side of bar to accept as touch
#define LEEWAY 10
// Amount touch can vary (in height px) before being 'let go'
#define THRESHOLD 50

namespace CustomElm {
    Slider::Slider(int x, int y, int w, int h, int bh) : Aether::Element(x, y, w, h) {
        this->bar = new Aether::RoundProgressBar(x + h/2, y + (h - bh)/2, w - h, bh);
        this->addElement(this->bar);
        this->knob = new Aether::Ellipse(x - 3, y - 3, h);
        this->setNudge(1);
        this->setValue(0.0);
        this->addElement(this->knob);
        this->setSelectable(true);
        this->setTouchable(true);
    }

    // Similar to Element::handleEvent but we have our own behaviour
    bool Slider::handleEvent(Aether::InputEvent * e) {
        bool b = false;

        // If it's a touch event jump to position
        if (e->type() == Aether::EventType::TouchPressed) {
            if (e->touchX() >= (this->x() - LEEWAY) && e->touchY() >= (this->y() - LEEWAY) && e->touchX() <= this->x() + this->w() + LEEWAY && e->touchY() <= this->y() + this->h() + LEEWAY && !this->hidden()) {
                this->setSelected(true);
                float val = 100.0 * (e->touchX() - this->x())/(float)this->w();
                if (val < 0.0) {
                    val = 0.0;
                } else if (val > 100.0) {
                    val = 100.0;
                }
                this->setValue(val);
                b = true;
            }

        } else if (e->type() == Aether::EventType::TouchMoved && this->selected()) {
            float val = 100.0 * (e->touchX() - this->x())/(float)this->w();
            if (val < 0.0) {
                val = 0.0;
            } else if (val > 100.0) {
                val = 100.0;
            }
            this->setValue(val);
            b = true;

        // Leave new value on release
        } else if (e->type() == Aether::EventType::TouchReleased && this->selected()) {
            if (e->touchX() < this->x()) {
                this->setValue(0.0);
            } else if (e->touchX() > this->x() + this->w()) {
                this->setValue(100.0);
            }
            this->setSelected(false);
            moveHighlight(this);
            if (this->callback() != nullptr) {
                this->callback()();
            }
            b = true;

        } else if (e->type() == Aether::EventType::ButtonPressed) {
            // Select on A down
            if (e->button() == Aether::Button::A && this->highlighted()) {
                this->setSelected(true);
                b = true;
            }

            // Otherwise move in direction
            if (this->selected()) {
                if (e->button() == Aether::Button::DPAD_RIGHT) {
                    this->setValue(this->value() + this->nudge);
                    b = true;
                } else if (e->button() == Aether::Button::DPAD_LEFT) {
                    this->setValue(this->value() - this->nudge);
                    b = true;
                }
            }

        } else if (e->type() == Aether::EventType::ButtonReleased) {
            if (e->button() == Aether::Button::A && this->selected()) {
                this->setSelected(false);
                if (this->callback() != nullptr) {
                    this->callback()();
                }
                b = true;
            }
        }

        // Note: Children don't handle events!
        return b;
    }

    // Need to override this in order to draw highlight on top of children
    void Slider::render() {
        // Do nothing if hidden or off-screen
        if (!this->isVisible()) {
            return;
        }

        SDL_BlendMode bld = SDLHelper::getBlendMode();
        if (this->selected()) {
            SDLHelper::setBlendMode(SDL_BLENDMODE_BLEND);
        }

        // Draw children
        for (size_t i = 0; i < this->children.size(); i++) {
            this->children[i]->render();
        }

        if (this->highlighted() && !this->isTouch) {
            SDLHelper::setBlendMode(SDL_BLENDMODE_BLEND);
            this->renderHighlighted();
        }

        SDLHelper::setBlendMode(bld);

        if (this->selected()) {
            this->renderSelected();
        }
    }

    float Slider::value() {
        return this->bar->value();
    }

    void Slider::setValue(float val) {
        this->bar->setValue(val);
        this->knob->setX(this->x() + ((this->w() - this->knob->xDiameter()) * (this->bar->value()/100.0)) - 3);
    }

    void Slider::setNudge(float n) {
        this->nudge = n;
    }

    void Slider::setBarBackgroundColour(Aether::Colour c) {
        this->bar->setBackgroundColour(c);
    }

    void Slider::setBarForegroundColour(Aether::Colour c) {
        this->bar->setForegroundColour(c);
    }

    void Slider::setKnobColour(Aether::Colour c) {
        this->knob->setColour(c);
    }

    void Slider::setW(int w) {
        Element::setW(w);
        this->bar->setW(this->w() - this->knob->xDiameter());
        this->bar->setX(this->x() + this->knob->xDiameter()/2);
        this->knob->setX(this->x() + ((this->w() - this->knob->xDiameter()) * (this->bar->value()/100.0)) - 3);
    }

    void Slider::setH(int h) {
        Element::setH(h);
        this->knob->setXDiameter(h);
        this->knob->setYDiameter(h);
        this->bar->setY(this->y() + (this->h() - this->bar->h())/2);
    }

    void Slider::setBarH(int bh) {
        this->bar->setH(bh);
        this->bar->setY(this->y() + (this->h() - this->bar->h())/2);
    }

    void Slider::renderHighlighted() {
        int midX = 2 + this->knob->x() + this->knob->xDiameter()/2;
        int midY = 2 + this->knob->y() + this->knob->yDiameter()/2;

        // Draw outline
        SDLHelper::drawEllipse(this->hiBorder, midX, midY, this->knob->xDiameter() + 2*this->hiSize, this->knob->yDiameter() + 2*this->hiSize);

        // Render an ellipse same colour as knob (this is a crude workaround but it works :D)
        SDLHelper::drawEllipse(this->knob->getColour(), midX, midY, this->knob->xDiameter(), this->knob->yDiameter());
    }

    void Slider::renderSelected() {
        int midX = 2 + this->knob->x() + this->knob->xDiameter()/2;
        int midY = 2 + this->knob->y() + this->knob->yDiameter()/2;

        SDLHelper::drawEllipse(this->hiSel, midX, midY, this->knob->xDiameter(), this->knob->yDiameter());
    }
};