/*
    This file is part of Lity project.
*/

#pragma once

#include <map>
#include <list>
#include <vector>
#include <boost/noncopyable.hpp>
#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/ast/ASTAnnotations.h>

namespace dev
{
namespace solidity
{

class ErrorReporter;
class NameAndTypeResolver;

/**
 * Resolves member references in fact field expressions.
 */

class FactMemberReferencesResolver: private ASTVisitor
{
public:
	FactMemberReferencesResolver(NameAndTypeResolver& _resolver): m_resolver(_resolver) {}

	/// @returns true if no errors during resolving and throws exceptions on fatal errors.
	bool resolve(ASTNode & _root);
	
	bool visit(FactDeclaration& _node) override;
	bool visit(FieldExpression& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(Conditional& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(Assignment& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(TupleExpression& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(UnaryOperation& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(BinaryOperation& _node) override{ m_stack.push_back(&_node); return true; }
	bool visit(FunctionCall& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(NewExpression& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(MemberAccess& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(IndexAccess& _node) override { m_stack.push_back(&_node); return true; }
	bool visit(Identifier& _node) override;

	void endVisit(FactDeclaration&) override;
	void endVisit(FieldExpression&) override { m_stack.pop_back(); }
	void endVisit(Conditional&) override { m_stack.pop_back(); }
	void endVisit(Assignment&) override { m_stack.pop_back(); }
	void endVisit(TupleExpression&) override { m_stack.pop_back(); }
	void endVisit(UnaryOperation&) override { m_stack.pop_back(); }
	void endVisit(BinaryOperation&) override { m_stack.pop_back(); }
	void endVisit(FunctionCall&) override { m_stack.pop_back(); }
	void endVisit(NewExpression&) override { m_stack.pop_back(); }
	void endVisit(MemberAccess&) override { m_stack.pop_back(); }
	void endVisit(IndexAccess&) override { m_stack.pop_back(); }


private:
	NameAndTypeResolver& m_resolver;
	bool m_errorOccurred = false;
	bool inFact = false;
	std::string m_factName;
	std::vector<Expression*> m_stack;
};

}
}
