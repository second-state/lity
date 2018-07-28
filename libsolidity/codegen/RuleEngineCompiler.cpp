
#include <utility>
#include <numeric>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <libdevcore/Common.h>
#include <libdevcore/SHA3.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/codegen/ExpressionCompiler.h>
#include <libsolidity/codegen/RuleEngineCompiler.h>
#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/LValue.h>
#include <libsolidity/codegen/ENIHandler.h>
#include <libevmasm/GasMeter.h>

#include <libdevcore/Whiskers.h>

using namespace std;

namespace dev
{
namespace solidity
{

void RuleEngineCompiler::appendFireAllRules(ContractDefinition const& _contract)
{
	// TODO: implementation
	(void)_contract;
	// call _contract.accept(*this) and traverse the rule AST,
	// refresh the set of facts in each alpha nodes,
	// actually fire the rules for each fact survived(?) in each terminal nodes, and so on.

	m_context << u256(0x111111111) << u256(0x54309531) << Instruction::SSTORE; // stub
}

void RuleEngineCompiler::appendFactInsert(TypePointer const& factType)
{
	// TODO: implementation
	// Be careful, we want to handle both memory and storage data location for the fact on stack top.

	for (size_t i = 0; i < factType->sizeOnStack(); i++) // stub
		m_context << Instruction::POP;
	m_context << u256(222222); // stub. Push factID into the stack
}

void RuleEngineCompiler::appendFactDelete()
{
	// TODO: implementation
	// 1. find out the factID's type.
	// 2. remove the fact from working memory.
	m_context << u256(0x333333333) << u256(0x54309531) << Instruction::SSTORE; // stub
	m_context << Instruction::POP; // stub
}

}
}
