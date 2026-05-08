// ValidatorPimpl.tpp — method definitions for ValidatorPimpl<Impl>.
// Include this file ONLY where Impl is a complete type, then add an
// explicit instantiation:  template class ValidatorPimpl<Impl>;

#include <memory>

namespace XML_Lib {

template<typename Impl>
ValidatorPimpl<Impl>::ValidatorPimpl(Node &xNode)
  : implementation(std::make_unique<Impl>(xNode)) {}

template<typename Impl>
ValidatorPimpl<Impl>::~ValidatorPimpl() = default;

template<typename Impl>
void ValidatorPimpl<Impl>::parse(ISource &source) { implementation->parse(source); }

template<typename Impl>
void ValidatorPimpl<Impl>::stringify(IDestination &dest) { implementation->stringify(dest); }

template<typename Impl>
void ValidatorPimpl<Impl>::validate(const Node &xNode) { implementation->validate(xNode); }

}// namespace XML_Lib
