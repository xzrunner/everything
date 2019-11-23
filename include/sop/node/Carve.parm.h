#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(FirstUEnable,  Boolean, firstu,  m_b_first_u,  (true))
PARAM_INFO(SecondUEnable, Boolean, secondu, m_b_second_u, (false))
PARAM_INFO(FirstVEnable,  Boolean, firstv,  m_b_first_v,  (false))
PARAM_INFO(SecondVEnable, Boolean, secondv, m_b_second_v, (false))

PARAM_INFO(FirstU,  Float, domainu1, m_first_u,  (0.0f))
PARAM_INFO(SecondU, Float, domainu2, m_second_u, (1.0f))
PARAM_INFO(FirstV,  Float, domainv1, m_first_v,  (0.0f))
PARAM_INFO(SecondV, Float, domainv2, m_second_v, (1.0f))
