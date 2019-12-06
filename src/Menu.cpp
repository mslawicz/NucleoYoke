/*
 * Menu.cpp
 *
 *  Created on: 25.10.2019
 *      Author: ep14231
 */

#include "Menu.h"
#include "System.h"

MenuItem::MenuItem(uint8_t pinUp, uint8_t pinDown) :
    pinUp(pinUp),
    pinDown(pinDown)
{
    actionUp = nullptr;
    actionDown = nullptr;
    pressedUp = false;
    pressedDown = false;
}

Menu::Menu() :
        menuItems{MenuItem(9, 8), MenuItem(11, 10), MenuItem(13, 12), MenuItem(15, 14)}
{

}

/*
 * handler of the display menu
 * to be called periodically in a loop
 */
void Menu::handler(void)
{
    for(auto& menuItem : menuItems)
    {
        menuItem.checkButtons();
    }
}

/*
 * check menu item buttons and executes the associated actions
 */
void MenuItem::checkButtons(void)
{
    auto inputSignals = 0; // XXX remainings after expander removal

    if(((inputSignals & (1 << pinUp)) != 0) &&
            (!pressedUp))
    {
        // button UP just pressed
        pressedUp = true;
        if(actionUp != nullptr)
        {
            actionUp();
        }
    }

    if(((inputSignals & (1 << pinUp)) == 0) &&
            (pressedUp))
    {
        // button UP just released
        pressedUp = false;
    }

    if(((inputSignals & (1 << pinDown)) != 0) &&
            (!pressedDown))
    {
        // button DOWN just pressed
        pressedDown = true;
        if(actionDown != nullptr)
        {
            actionDown();
        }
    }

    if(((inputSignals & (1 << pinDown)) == 0) &&
            (pressedDown))
    {
        // button DOWN just released
        pressedDown = false;
    }
}

/*
 * set menu item
 */
void Menu::setItem(uint8_t index, std::string text, std::function<void(void)> actionUp,
        std::function<void(void)> actionDown)
{
    if(index >= NoOfMenuItems)
    {
        return;
    }
    menuItems[index].setActionUp(actionUp);
    menuItems[index].setActionDown(actionDown);
    System::getInstance().getDisplay()->drawRectangle(index*32, 54, 31, 10);
    int8_t startX = (31 - System::getInstance().getDisplay()->calculateLength(text, FontArial9)) / 2;
    if(startX < 0)
    {
        startX = 0;
    }
    if(text.size() > 0)
    {
        System::getInstance().getDisplay()->print(index * 32 + static_cast<uint8_t>(startX), 55, text, FontArial9, true, false, index * 32 + 31);
    }
}
