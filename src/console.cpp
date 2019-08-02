/*
 * console.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "console.h"
#include <unordered_map>

const bool Console::IsChannelActive[] = {
        true, //LC_SYSTEM,
        true, //LC_CONSOLE,
        true, //LC_I2C,
        true, //LC_SPI,
        true, //LC_USB
};

Console::Console() :
    interface(USART3, 115200)
{
}

Console::~Console()
{
}

/*
 * console handler to be called periodically in a loop
 */
void Console::handler(void)
{
    // handle a received string
    if(interface.stringReceived())
    {
        // there is a message in reception queue
        auto message = interface.getReceivedString();
        // echo CR LF
        interface.send("\r\n");

        // interpret command here
        // XXX send back the string for testing
        sendMessage(Severity::Debug, LogChannel::LC_CONSOLE, "received: '" + message + "'");

        // send the prompt character
        sendPrompt();
    }

    // handle sending queue
    interface.transmitHandler();
}

/*
 * send log message to console
 */
void Console::sendMessage(Severity level, LogChannel channel, std::string message)
{
    const std::unordered_map<Severity, std::string> severityStrings =
    {
            {Error, "error"},
            {Warning, "warning"},
            {Info, "info"},
            {Debug, "debug"}
    };

    if(Console::IsChannelActive[channel])
    {
        message = severityStrings.find(level)->second + ": " + message + "\r\n";
        interface.send(message);
    }
}

/*
 * print hex number to std::string
 * positions - minimal digits, 0=auto
 * prefix - adds '0x' at the beginning
 */
std::string Console::toHex(uint32_t value, uint8_t positions, bool prefix)
{
    std::string hexString;
    const std::string digits = "0123456789ABCDEF";
    do
    {
        hexString.insert(0, 1, digits[value & 0x0F]);
        value >>= 4;
        if(positions>0)
        {
            positions--;
        }
    } while((value>0) || (positions>0));
    if(prefix)
    {
        hexString.insert(0, "0x");
    }
    return hexString;
}
