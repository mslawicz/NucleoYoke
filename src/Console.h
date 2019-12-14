/*
 * console.h
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "UART.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <functional>

enum class Severity : uint8_t
{
    Error,
    Warning,
    Info,
    Debug
};

enum LogChannel
{
    LC_SYSTEM,
    LC_CONSOLE,
    LC_I2C,
    LC_SPI,
    LC_USB,
    LC_ADC,
    LC_EXP,
    LC_SERVO,
    LC_END
};

using StringVector = std::vector<std::string>;
using CommandContainer = std::unordered_map<std::string, std::pair<std::string, std::function<void(StringVector)>>>;

class Console
{
public:
    Console();
    ~Console();
    UART& getInterface(void) { return interface; }
    void handler(void);
    void sendPrompt(void) { interface.send(">"); }
    void sendMessage(Severity level, LogChannel channel, std::string message);
    void registerCommands(void);
    static const bool IsChannelActive[LogChannel::LC_END];
    static Severity systemLevel;
private:
    void executeCommand(std::string commandString);
    void displayHelp(StringVector);
    StringVector splitString(std::string str);
    UART interface;
    CommandContainer commands;
};

#endif /* CONSOLE_H_ */
