/*
 * Conversion.cpp
 *
 *  Created on: 13.10.2019
 *      Author: Marcin
 */

#include "Conversion.h"

/*
 * convert hex number to std::string
 * positions - minimal digits, 0=auto
 * prefix - adds '0x' at the beginning
 */
std::string toHex(uint32_t value, uint8_t positions, bool prefix)
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


