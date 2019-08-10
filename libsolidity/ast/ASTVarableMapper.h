#pragma once

#include <map>
#include <string>
#include <libsolidity/ast/ASTVisitor.h>

namespace dev
{
namespace solidity
{

class ASTVarableMapper: public ASTConstVisitor
{
public:

	ASTVarableMapper(
		ASTNode const& _ast
	);

	void buildMapping(std::map<const ASTNode*, std::string>& _map);

	bool visit(ContractDefinition const& _node) override;
	bool visit(VariableDeclaration const& _node) override;

private:
	ASTNode const* m_ast;
	std::map<const ASTNode*, std::string> *m_map;
    std::string m_contract;
};

}
}
