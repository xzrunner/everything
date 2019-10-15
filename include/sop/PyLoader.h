#pragma once

namespace sop
{

class PyLoader
{
public:
    PyLoader();
    ~PyLoader();

    void RunFile(const char* filepath);

}; // PyLoader

}