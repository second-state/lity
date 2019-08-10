#include <map>
#include <string>
#include <libsolidity/ast/ASTVarableMapper.h>
#include <libsolidity/ast/AST.h>

namespace dev
{
namespace solidity
{

ASTVarableMapper::ASTVarableMapper(
	ASTNode const& _ast
):m_ast(&_ast)
{
}

void ASTVarableMapper::buildMapping(std::map<const ASTNode*, std::string>& _map)
{
	m_map = &_map;
	m_ast->accept(*this);
	m_map = nullptr;
}

bool ASTVarableMapper::visit(ContractDefinition const& _node){
	m_contract = _node.name();
	return true;
}
bool ASTVarableMapper::visit(VariableDeclaration const& _node){
	(*m_map)[&_node] = m_contract;
	return true;
}

}
}
