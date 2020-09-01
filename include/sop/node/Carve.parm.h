#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(FirstUEnable,  bool, firstu,  m_b_first_u,  (true))
PARAM_INFO(SecondUEnable, bool, secondu, m_b_second_u, (false))
PARAM_INFO(FirstVEnable,  bool, firstv,  m_b_first_v,  (false))
PARAM_INFO(SecondVEnable, bool, secondv, m_b_second_v, (false))

PARAM_INFO(FirstU,  float, domainu1, m_first_u,  (0.0f))
PARAM_INFO(SecondU, float, domainu2, m_second_u, (1.0f))
PARAM_INFO(FirstV,  float, domainv1, m_first_v,  (0.0f))
PARAM_INFO(SecondV, float, domainv2, m_second_v, (1.0f))
