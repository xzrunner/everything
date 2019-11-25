#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,    Int2, size, m_size,    (10, 10))
PARAM_INFO(Rows,    Int,  rows, m_rows,    (10))
PARAM_INFO(Columns, Int,  cols, m_columns, (10))
