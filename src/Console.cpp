/*
 * console.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "Console.h"
#include "System.h"
#include "Yoke.h"

#include <unordered_map>

const bool Console::IsChannelActive[] = {
        true, //LC_SYSTEM,
        true, //LC_CONSOLE,
        true, //LC_I2C,
        true, //LC_SPI,
        true, //LC_USB
        true, //LC_ADC
        true  //LC_EXP
};

Severity Console::systemLevel = Severity::Debug;

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
        // execute the console command
        executeCommand(message);
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

    if(Console::IsChannelActive[channel] &&
            (level <= Console::systemLevel))
    {
        message = severityStrings.find(level)->second + ": " + message + "\r\n";
        interface.send(message);
    }
}

/*
 * execute console command
 */
void Console::executeCommand(std::string commandString)
{
    interface.send("\r\n");
    auto commandVector = splitString(commandString);
    if(!commandVector[0].empty())
    {
        // command not empty
        auto commandIt = commands.find(commandVector[0]);
        if(commandIt == commands.end())
        {
            sendMessage(Severity::Info,LogChannel::LC_CONSOLE, "unknown command");
        }
        else
        {
            commandIt->second.second(commandVector);
        }
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
    for(auto command : commands)
    {
        interface.send(command.second.first + "\r\n");
    }
}

/*
 * register console commands
 * it must be called after creation of all essential objects
 */
void Console::registerCommands(void)
{
    commands.emplace("help", std::pair<std::string, std::function<void(StringVector)>>({"help - command help", [&](StringVector arguments){this->displayHelp(arguments);}}));
    commands.emplace("ff", std::pair<std::string, std::function<void(StringVector)>>({"ff - display force feedback current data", std::bind(&Yoke::displayForceFeedbackData, System::getInstance().getYoke())}));
}
