#pragma once

#include <string>

namespace sop
{

class Evaluator;

class PyLoader
{
public:
    PyLoader(Evaluator& eval);
    ~PyLoader();

    void RunFile(const std::string& filepath);

private:
    void PrepareContext();

private:
    Evaluator& m_eval;

}; // PyLoader

}