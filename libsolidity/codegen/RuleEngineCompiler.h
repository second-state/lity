#pragma once

#include <functional>
#include <memory>
#include <boost/noncopyable.hpp>
#include <libdevcore/Common.h>
#include <libevmasm/SourceLocation.h>
#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/ast/ASTPrinter.h>
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


// node of Rete Network
class ReteNode : boost::noncopyable
{
public:
	ReteNode(): m_out(ReteNode::serialNo++) {}
	void setOutAddr(dev::u256 _out) { m_out = _out; }
	dev::u256 outAddr() const { return m_out; }
	virtual void print() const {}
	// for topological sort
	virtual std::vector<ReteNode const*> parents() const { return std::vector<ReteNode const*>(); }
protected:
	dev::u256 m_out;
	static dev::u256 serialNo;
};

class TypeNode: public ReteNode
{
public:
	TypeNode(TypePointer _type): m_type(_type) {}
	TypePointer type() const { return m_type; }
	void setType(TypePointer _type) { m_type = _type; }
	virtual void print() const override
	{
		std::cerr << "TypeNode : " << this << "\n";
		std::cerr << "  outAddr: " << m_out << "\n";
		std::cerr << "  " << m_type->richIdentifier()+"\n";
	}
private:
	TypePointer m_type;
};

// input of alphaNode is typeNode
class AlphaNode: public ReteNode
{
public:
	AlphaNode(FactDeclaration const& _fact): m_parent(nullptr), m_fact(_fact) {}
	TypeNode const* parent() const { return m_parent; }
	FactDeclaration const& fact() const{ return m_fact; }
	const std::vector<FieldExpression const*>& exprs() const { return m_exprs; }
	void addExpr(FieldExpression const& _fieldExpr) { m_exprs.push_back(&_fieldExpr); }
	void setPar(TypeNode const& _parent){ m_parent = &_parent; }
	virtual void print() const override
	{
		std::cerr << "AlphaNode: " << this << "\n";
		std::cerr << "  outAddr: " << m_out << "\n";
		std::cerr << "  par    : " << m_parent << "\n";
		for(auto expr: m_exprs)
		{
			ASTPrinter printer(*expr);
			printer.print(std::cerr);
		}
	}
	virtual std::vector<ReteNode const*> parents() const override
	{
		std::vector<ReteNode const*> pars;
		pars.push_back((ReteNode const*) m_parent);
		return pars;
	}
private:
	TypeNode const* m_parent;
	std::vector<FieldExpression const*> m_exprs;
	FactDeclaration const& m_fact;
};

class JoinNode: public ReteNode
{
public:
	JoinNode(): m_leftParent(nullptr), m_rightParent(nullptr) {}
	JoinNode const* leftParent() const { return m_leftParent; }
	AlphaNode const* rightParent() const { return m_rightParent; }
	const std::vector<FieldExpression const*>& exprs() const { return m_exprs; }
	void setLeftPar(JoinNode const& _node) { m_leftParent = &_node; }
	void setRightPar(AlphaNode const& _node) { m_rightParent = &_node; }
	void addExpr(FieldExpression const& _fieldExpr) { m_exprs.push_back(&_fieldExpr); }
	void addFact(FactDeclaration const& _fact) { m_facts.push_back(&_fact); }
	int tupeSize() const { return m_facts.size(); }
	FactDeclaration const& fact(int i) const { return *m_facts[i]; }
	virtual void print() const override
	{
		std::cerr << "JoinNode: " << this << "\n";
		std::cerr << "  outAddr     : " << m_out << "\n";
		std::cerr << "  left (beta ):" << m_leftParent << "\n";
		std::cerr << "  right(alpha): " << m_rightParent << "\n";
		std::cerr << "  tupleSize   : " << m_facts.size() << "\n";
		for(auto expr: m_exprs)
		{
			ASTPrinter printer(*expr);
			printer.print(std::cerr);
		}
	}
	virtual std::vector<ReteNode const*> parents() const override
	{
		std::vector<ReteNode const*> pars;
		if(m_leftParent!=nullptr) pars.push_back((ReteNode const*) m_leftParent);
		pars.push_back((ReteNode const*) m_rightParent);
		return pars;
	}
private:
	JoinNode const* m_leftParent;
	AlphaNode const* m_rightParent;
	std::vector<FieldExpression const*> m_exprs;
	std::vector<FactDeclaration const*> m_facts;
};

// terminal node for a certain rule
class TermNode: public ReteNode
{
public:
	TermNode(Rule const& _rule): m_rule(_rule) {}
	JoinNode const* parent() const { return m_parent; }
	Rule const& rule() { return m_rule; }
	void setPar(JoinNode const& _parent) { m_parent = &_parent; }
	virtual void print() const override
	{
		std::cerr << "TermNode: " << this << "\n";
		std::cerr << "  outAddr: " << m_out << "\n";
		std::cerr << "  rule   : " << m_rule.name() << "\n";
	}
	virtual std::vector<ReteNode const*> parents() const override
	{
		std::vector<ReteNode const*> pars;
		pars.push_back((ReteNode const*) m_parent);
		return pars;
	}
private:
	JoinNode const* m_parent;
	Rule const& m_rule;
};

/**
 * Compiler for Lity rule engine
 */
class RuleEngineCompiler: private ASTConstVisitor
{
public:
	explicit RuleEngineCompiler(CompilerContext& _compilerContext): m_context(_compilerContext) { }

	/// Compiles rule engine network.
	/// Note that then block is compiled by ContractCompiler.
	eth::AssemblyItem compileNetwork(Rule const&);

	/// Appends jump to allRules
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

	/// List of WMEs to be executed
	/// The list shall be filled after rule filter phase.
	u256 terminalNodeInListPtr() const { return m_termNode->outAddr(); }
	/// List of WMEs that already executed
	/// This is used for lock-on-active attribute.
	u256 lockOnActListPtr(Rule const& _rule) const { return keccak256(_rule.name()+"-lockOnActive-list"); }
	u256 noLoopAddr() const { return keccak256("noLoopHashLoc"); }

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

	/// Stack pre : factHash
	/// Stack post: markerStorageAddr
	void appendGetLockOnActiveMapMarker(Rule const& _rule) const { m_context << keccak256(_rule.name()) << Instruction::XOR; }

	/// Stack pre : factHash
	/// Stack post: noLoopHash
	void appendGetNoLoopHash(Rule const& _rule) const { m_context << keccak256(_rule.name()) << Instruction::XOR; }

private:
	// gen code from network
	void compile(TypeNode const&);
	void compile(AlphaNode const&);
	void compile(JoinNode const&);
	void compile(TermNode const&);
	// gen network from AST
	bool visit(Rule const&) override;
	bool visit(FactDeclaration const&) override;
	bool visit(FieldExpression const&) override;

	void endVisit(Rule const&) override;
	void endVisit(FactDeclaration const&) override;
	void endVisit(FieldExpression const&) override;

	// gen node execution order
	void genOrder()
	{
		if(m_ReteNodeOrder.size()==0)
			genOrderRecursive((ReteNode const*)m_termNode);
	}
	void genOrderRecursive(ReteNode const* _node)
	{
		for(ReteNode const* par: _node->parents())
			genOrderRecursive(par);
		m_ReteNodeOrder.push_back(_node);
	}
	eth::AssemblyItem entryNode(ReteNode const& _node)
	{
		if (0 == m_entryNode.count(&_node))
			m_entryNode.insert(std::make_pair(&_node, m_context.newTag()));
		return m_entryNode.find(&_node)->second;
	}
	TypeNode& typeNode(TypePointer type)
	{
		if(0==m_typeNodes.count(type))
			m_typeNodes.insert(std::make_pair(type, new TypeNode(type)));
		return *m_typeNodes[type];
	}
	AlphaNode& alphaNode(FactDeclaration const& _fact)
	{
		if(0==m_alphaNodes.count(&_fact))
			m_alphaNodes.insert(std::make_pair(&_fact, new AlphaNode(_fact)));
		return *m_alphaNodes[&_fact];
	}
	JoinNode& joinNode(FactDeclaration const& _fact)
	{
		if(0==m_joinNodes.count(&_fact))
			m_joinNodes.insert(std::make_pair(&_fact, new JoinNode()));
		return *m_joinNodes[&_fact];
	}
	TermNode& termNode()
	{
		if(m_termNode==nullptr)
			m_termNode = new TermNode(*m_currentRule);
		return *m_termNode;
	}

	std::vector<dev::u256> getAllNodeOutListPtrAddr()
	{
		std::vector<dev::u256> ptsAddrs;
		for(auto pr: m_typeNodes)
			ptsAddrs.push_back(pr.second->outAddr());
		for(auto pr: m_alphaNodes)
			ptsAddrs.push_back(pr.second->outAddr());
		ptsAddrs.push_back(m_termNode->outAddr());
		return ptsAddrs;
	}

	void appendFact(FactDeclaration const & _fact);
	// TODO: use ArrayUtils
	void appendPushItemToStorageArray(h256 _itemAddr);
	void appendDeleteItemInStorageArray();
	void appendAccessIndexStorage();
	void appendWriteIndexStorage();

	void appendAssertNoRuleEngineLock();
	void appendAssertHaveRuleEngineLock();


	h256 getRuleEngineLockLocation() const { return keccak256("__lityRuleEngineLock~~__"); }

	Rule const* m_currentRule;
	FactDeclaration const* m_currentFact;
	int m_currentFieldNo=0;

	/// magic constant used for create a mapping from factID to typeList
	h256 getIdToListXorMask() const { return keccak256("__idToListXorMask~~__"); }

	CompilerUtils utils();

	h256 getRuleEngineReevaluateLocation() const { return keccak256("__ruleEngineReevaluationLoc~~__"); }

	CompilerContext& m_context;

	std::map<FactDeclaration const*, std::vector<FieldExpression const*>> m_alphaExprs;
	std::map<FactDeclaration const*, std::vector<FieldExpression const*>> m_betaExprs;
	std::map<TypePointer, TypeNode*> m_typeNodes;
	std::map<FactDeclaration const*, AlphaNode*> m_alphaNodes;
	std::map<FactDeclaration const*, JoinNode*> m_joinNodes;
	TermNode* m_termNode=nullptr;
	std::map<ReteNode const*, eth::AssemblyItem> m_entryNode;
	std::vector<ReteNode const*> m_ReteNodeOrder;
};

class ExprUtils: private ASTConstVisitor
{
public:
	ExprUtils(Expression const& _expr): m_expr(_expr) {}
	int numOfFacts()
	{
		m_expr.accept(*this);
		return facts.size();
	}
	bool isAlpha() { return numOfFacts()==1; }
	virtual void endVisit(Identifier const& _identifier) override
	{
		Declaration const* declaration = _identifier.annotation().referencedDeclaration;
		if (auto fact = dynamic_cast<FactDeclaration const*>(declaration))
			facts.insert(fact);
	}
private:
	std::set<FactDeclaration const*> facts;
	Expression const& m_expr;
};

}
}
