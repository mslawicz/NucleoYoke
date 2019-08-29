/*
 * console.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "Console.h"

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
    // register console commands
    commands.emplace("h", std::pair<std::string, std::function<void(StringVector)>>({"command help", [&](StringVector arguments){this->displayHelp(arguments);}}));
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
        // execute the console command
        executeCommand(message);
        // echo CR LF
        interface.send("\r\n");

        // interpret command here

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

/*
 * execute console command
 */
void Console::executeCommand(std::string commandString)
{
    auto commandVector = splitString(commandString);
    // display vector for test
    for(auto word : commandVector)
    {
        sendMessage(Severity::Info,LogChannel::LC_CONSOLE, "Command substring '" + word + "'");
    }
}

/*
 * split string by space delimiter
 * returns the vector of splitted words
 * delimiter should be a single space character only
 */
StringVector Console::splitString(std::string str)
{
    StringVector words;
    size_t spacePosition;
    do
    {
        spacePosition = str.find(' ');
        if(spacePosition != std::string::npos)
        {
            words.push_back(str.substr(0, spacePosition));
            str = str.substr(spacePosition+1);
        }
    } while (spacePosition != std::string::npos);
    words.push_back(str);
    return words;
}

/*
 * display command list help
 *
 */
void Console::displayHelp(StringVector arguments)
{
    sendMessage(Severity::Info,LogChannel::LC_CONSOLE, "lets display help here");
}
