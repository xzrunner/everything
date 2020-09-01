#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(IterMethod,   ForeachPrimEnd::IterMethod,   itermethod, m_iter_method,   (ForeachPrimEnd::IterMethod::AutoDetectFromInputs))
PARAM_INFO(GatherMethod, ForeachPrimEnd::GatherMethod, method,     m_gather_method, (ForeachPrimEnd::GatherMethod::Feedback))

PARAM_INFO(PieceElements,  ForeachPrimEnd::PieceElements, class,     m_piece_elements, (ForeachPrimEnd::PieceElements::Points))
PARAM_INFO(UsePieceAttrib, bool,                          useattrib, m_use_attrib,     (false))

PARAM_INFO(DefaultBlockPath, std::string, blockpath,    m_block_path,    (""))
PARAM_INFO(PieceBlockPath,   std::string, templatepath, m_template_path, (""))

PARAM_INFO(DoSinglePass,     bool, dosinglepass, m_do_single_pass,     (false))
PARAM_INFO(SinglePassOffset, int,  singlepass,   m_single_pass_offset, (0))
