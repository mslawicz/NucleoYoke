/*
 * Menu.h
 *
 *  Created on: 25.10.2019
 *      Author: ep14231
 */

#ifndef MENU_H_
#define MENU_H_

#include <string>
#include <functional>

class MenuItem
{
public:
    MenuItem(uint8_t pinUp, uint8_t pinDown);
    void checkButtons(void);
    void setActionUp(std::function<void(void)> action) { actionUp = action; }
    void setActionDown(std::function<void(void)> action) { actionDown = action; }
private:
    std::function<void(void)> actionUp;
    std::function<void(void)> actionDown;
    bool pressedUp;
    bool pressedDown;
    uint8_t pinUp;
    uint8_t pinDown;
};

class Menu
{
public:
    Menu();
    void handler(void);
    void setItem(uint8_t index, std::string text, std::function<void(void)> actionUp, std::function<void(void)> actionDown);
private:
    static const uint8_t NoOfMenuItems = 4;
    MenuItem menuItems[NoOfMenuItems];
};

#endif /* MENU_H_ */
