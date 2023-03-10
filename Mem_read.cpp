//#include <bits/stdc++.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include <string.h>
#include <cctype>
#include <cstring>

int main()
{
    int gp = { 30 };
    uint8_t memory_array[65536] = {};
    int byte1, byte2, byte3, byte4;
    uint32_t memory_content;
    
    byte1 = memory_array[gp];
    byte2 = memory_array[gp + 1] << 8;
    byte3 = memory_array[gp + 2] << 16;
    byte4 = memory_array[gp + 3] << 24;
    memory_content = byte1 | byte2 | byte3 | byte4;
    return memory_content;
}
