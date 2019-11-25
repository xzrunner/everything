#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,      Float2, size,     m_size,     (1.0f, 1.0f))
PARAM_INFO(SplitSize, Float2, split_sz, m_split_sz, (0.5f, 0.5f))
PARAM_INFO(MinSize,   Float2, min_sz,   m_min_sz,   (0.25f, 0.25f))

PARAM_INFO(Seed, Int, seed, m_seed, (0))
