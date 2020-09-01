#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,    sm::ivec2, size, m_size,    (10, 10))
PARAM_INFO(Rows,    int,       rows, m_rows,    (10))
PARAM_INFO(Columns, int,       cols, m_columns, (10))
