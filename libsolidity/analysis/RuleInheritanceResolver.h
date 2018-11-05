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

class RuleInheritanceResolver: private ASTVisitor
{
public:
	RuleInheritanceResolver(NameAndTypeResolver& _resolver): m_resolver(_resolver) {}

	/// @returns true if no errors during resolving and throws exceptions on fatal errors.
	bool resolve(ASTNode& _root);

	bool visit(Rule& _node) override;

private:
	NameAndTypeResolver& m_resolver;
	bool m_errorOccurred = false;
};

}
}
