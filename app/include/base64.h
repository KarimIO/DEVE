//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00 - MODIFIED BY MOHAMED GABER FOR DEVE
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <Types.h>
#include <vector>
#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::vector<uint8> base64_decode(std::string const& s);

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */