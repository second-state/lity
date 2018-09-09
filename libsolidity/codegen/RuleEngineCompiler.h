#pragma once

#include <functional>
#include <memory>
#include <boost/noncopyable.hpp>
#include <libdevcore/Common.h>
#include <libevmasm/SourceLocation.h>
#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/codegen/LValue.h>
#include <libsolidity/interface/Exceptions.h>
#include <libevmasm/Instruction.h>
#include <libevmasm/Assembly.h>
#include <libevmasm/GasMeter.h>
#include <libsolidity/codegen/ContractCompiler.h>
#include <libsolidity/inlineasm/AsmCodeGen.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/interface/ErrorReporter.h>
#include <libsolidity/codegen/ExpressionCompiler.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/DynArrUtils.h>

namespace dev {
namespace solidity {

/**
 * Compiler for Lity rule engine
 */
class RuleEngineCompiler: private ASTConstVisitor
{
public:
	explicit RuleEngineCompiler(CompilerContext& _compilerContext): m_context(_compilerContext) { }

	/// Compiles rule engine network.
	/// Note that then block is compiled by ContractCompiler.
	eth::AssemblyItem compileNodes(Rule const& _rule);

	/// Appends inline code to fire all rules
	void appendFireAllRules(ContractDefinition const& _contract);

	/// Insert a fact into working memory.
	/// stack pre: fact
	/// stack post: factID
	/// @param factType the type of the fact
	void appendFactInsert(TypePointer const& factType);

	/// Delete a fact from working memory.
	/// stack pre: factID
	/// stack post:
	void appendFactDelete();

	/// Mark a fact as updated. In current implementation, rule engine will be restarted.
	/// stack pre: fact
	/// stack post:
	void appendUpdate();

	dev::u256 terminalNodeInListPtr() const { return m_nodeOutListPtrAddr.back(); }

	/// Clean up Rete network nodes
	/// stack pre:
	/// stack post:
	void appendCleanUpNodes();

	void pushWhetherNeedReevaluation() { m_context << getRuleEngineReevaluateLocation() << Instruction::SLOAD; }
	void resetReevaluationMarker() { m_context << 0 << getRuleEngineReevaluateLocation() << Instruction::SSTORE; }

	/// We want to prevent user from using factInsert/factDelete/fireAllRules
	/// inside then-block.
	void appendLockRuleEngineOrFail();
	void appendUnlockRuleEngine();
	void appendAssertNoRuleEngineLock();
	void appendAssertHaveRuleEngineLock();

private:
	bool visit(Rule const&) override;
	bool visit(FactDeclaration const&) override;
	bool visit(FieldExpression const&) override;

	void endVisit(Rule const&) override;
	void endVisit(FactDeclaration const&) override;
	void endVisit(FieldExpression const&) override;

	// TODO: use ArrayUtils
	void appendPushItemToStorageArray(h256 _itemAddr);
	void appendDeleteItemInStorageArray();
	void appendAccessIndexStorage();
	void appendWriteIndexStorage();

	h256 getRuleEngineLockLocation() const { return keccak256("__lityRuleEngineLock~~__"); }

	const Rule* m_currentRule;
	const FactDeclaration* m_currentFact;
	int m_currentFieldNo=0;

	/// magic constant used for create a mapping from factID to typeList
	h256 getIdToListXorMask() const { return keccak256("__idToListXorMask~~__"); }

	CompilerUtils utils();
	// pointer (in storage) to factList of Node
	std::vector<dev::u256> m_nodeOutListPtrAddr;

	h256 getRuleEngineReevaluateLocation() const { return keccak256("__ruleEngineReevaluationLoc~~__"); }

	CompilerContext& m_context;
};

}
}
