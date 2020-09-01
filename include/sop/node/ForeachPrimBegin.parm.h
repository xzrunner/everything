#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Method,    ForeachPrimBegin::Method, method,    m_method,     (ForeachPrimBegin::Method::Feedback))
PARAM_INFO(BlockPath, std::string,                   blockpath, m_block_path, (""))
