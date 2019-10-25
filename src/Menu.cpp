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
        menuItems{MenuItem(1,2), MenuItem(3,4), MenuItem(5,6), MenuItem(7,8)}   // TODO provide actual pins here
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
    auto inputSignals = System::getInstance().getGpioExpanders()[2]->getInputRegister();

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
    // TODO display menu text here
}
