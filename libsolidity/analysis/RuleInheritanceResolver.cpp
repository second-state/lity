#include <libsolidity/analysis/RuleInheritanceResolver.h>
#include <libsolidity/analysis/NameAndTypeResolver.h>
#include <libsolidity/interface/ErrorReporter.h>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dev::solidity;

bool RuleInheritanceResolver::resolve(ASTNode & _root)
{
	_root.accept(*this);
	return !m_errorOccurred;
}

bool RuleInheritanceResolver::visit(Rule& _rule)
{
	auto rules = m_resolver.rules();
	m_resolver.rules()[_rule.name()] = &_rule;
	// from bottom to top ancestor
	for(Rule const* baseRule = rules[_rule.baseRuleName()]; baseRule != nullptr; baseRule = rules[baseRule->baseRuleName()])
	{
		auto const& baseFacts = baseRule->facts();
		for(auto fit = baseFacts.rbegin(); fit != baseFacts.rend(); fit++)
		{
			auto const& fact = *fit;
			std::vector<ASTPointer<FieldExpression>> fieldExprsCopy;
			for(auto const& fieldExpr: fact->fieldExpressions())
				fieldExprsCopy.push_back( fieldExpr->fieldDeepCopy() );
			_rule.addFactInFront(
				make_shared<FactDeclaration>(
					fact->location(),
					make_shared<ASTString>(fact->name()),
					fact->typeName(),
					fieldExprsCopy
				)
			);
		}
	}
	return false;
}
