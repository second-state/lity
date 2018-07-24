#include <libsolidity/analysis/FactMemberReferencesResolver.h>
#include <libsolidity/analysis/NameAndTypeResolver.h>
#include <libsolidity/interface/ErrorReporter.h>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dev::solidity;

bool FactMemberReferencesResolver::resolve(ASTNode & _root)
{
	_root.accept(*this);
	return !m_errorOccurred;
}

bool FactMemberReferencesResolver::visit(FactDeclaration& _fact)
{
	inFact = true;
	m_factName = _fact.name();
	m_resolver.setScope(&_fact);
	return true;
}

void FactMemberReferencesResolver::endVisit(FactDeclaration&)
{
	inFact = false;
	return;
}

bool FactMemberReferencesResolver::visit(Identifier& _identifier)
{
	if(inFact)
	{
		auto declarations = m_resolver.nameFromCurrentScope(_identifier.name());
		if (declarations.empty())
		{
			Expression* parent = m_stack.back();
			auto node = make_shared<MemberAccess>(_identifier.location(), make_shared<Identifier>(_identifier.location(), make_shared<ASTString>(m_factName)), make_shared<ASTString>(_identifier.name()));
			parent->replaceChild(&_identifier, node);
		}
	}
	return true;
}
