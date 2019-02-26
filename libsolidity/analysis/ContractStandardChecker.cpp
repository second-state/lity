/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libsolidity/analysis/ContractStandardChecker.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/interface/ErrorReporter.h>

using namespace std;
using namespace dev;
using namespace dev::solidity;

ContractStandardChecker::ContractStandardChecker
(
	ErrorReporter& _errorReporter,
	boost::optional<std::string> _contractStandardStr
): m_errorReporter(_errorReporter)
{
	if (_contractStandardStr)
	{
		if (*_contractStandardStr == "ERC20")
			m_contractStandards.push_back(make_shared<ERC20ContractStandard>());
		else if (*_contractStandardStr == "ERC223")
			m_contractStandards.push_back(make_shared<ERC223ContractStandard>());
		else if (*_contractStandardStr == "ERC721")
			m_contractStandards.push_back(make_shared<ERC721ContractStandard>());
		else if (*_contractStandardStr == "ERC827")
			m_contractStandards.push_back(make_shared<ERC827ContractStandard>());
		else if (*_contractStandardStr == "ERC884")
			m_contractStandards.push_back(make_shared<ERC884ContractStandard>());
		else
			solAssert(false, "Unknown contract standard");
	}
}

bool ContractStandardChecker::checkContractStandard(ContractDefinition const& _contract)
{
	for (auto cs: m_contractStandards)
	{
		if (cs->checkCompatibility(_contract, m_errorReporter, m_mode))
			m_errorReporter.info(_contract.location(), _contract.name() + " is compatible to " + cs->name() + ".");
	}
	return true; // do not regard contract standard detection as an error, returning true
}

bool BasicContractStandard::visit(ContractDefinition const& _contract)
{
	for (ContractDefinition const* contract: _contract.annotation().linearizedBaseContracts)
	{
		for (FunctionDefinition const* function: contract->definedFunctions())
		{
			m_touch(function);
			function->accept(*this); // we need to visit all functions defined/inherited in the contract
		}
		for (EventDefinition const* event: contract->events())
			m_touch(event);
	}
	return false;
}

bool BasicContractStandard::visit(MemberAccess const& _ma)
{
	m_touch(&_ma);
	return true;
}

// checks if all _requiredFunctions presents in _contract
bool BasicContractStandard::checkFunctionExistence
(
	ContractDefinition const& _contract,
	FunctionSpecifications const& _requiredFunctions,
	ErrorReporter& _reporter,
	ContractStandardChecker::Mode const _mode
)
{
	vector<pair<string, shared_ptr<FunctionType const>>> missingFunctions;
	for(pair<string, shared_ptr<FunctionType const>> p: _requiredFunctions)
	{
		bool found = false;
		// m_touch is a callback function that will be called when ast nodes are visited
		m_touch = [&found, &p](ASTNode const* _astNode) {
			if(auto fd = dynamic_cast<FunctionDefinition const*>(_astNode))
			{
				FunctionTypePointer ftp = fd->functionType(false /* we only consider external functions */);
				if(ftp && fd->name() == p.first && *ftp == *p.second)
					found = true;
			}
			if(auto ed = dynamic_cast<EventDefinition const*>(_astNode))
			{
				FunctionTypePointer ftp = ed->functionType(true /* Events are internal */);
				if(ftp && ed->name() == p.first && *ftp == *p.second)
					found = true;
			}
		};
		_contract.accept(*this);
		if (!found) {
			switch (_mode)
			{
			case ContractStandardChecker::WarnAll:
				_reporter.info(
					_contract.location(),
					"Missing '" + p.first + "' with type signature '" + (p.second->toString(false)) + "'. " +
						_contract.name() + " is not compatible to " + this->name() + "."
				);
				return false;
			case ContractStandardChecker::WarnIfAtLeastOneFunctionImplemented:
				missingFunctions.push_back(p);
				break;
			default:
				solAssert(false, "Unknown contract standard checker mode.");
			}
		}
	}
	switch (_mode)
	{
	case ContractStandardChecker::WarnAll:
		return true;
	case ContractStandardChecker::WarnIfAtLeastOneFunctionImplemented:
		if (missingFunctions.size() == _requiredFunctions.size())
			return false; // All required functions not implemented
		if (missingFunctions.size() == 0)
			return true; // All required function implemented
		for (pair<string, shared_ptr<FunctionType const>> p: missingFunctions)
			_reporter.info(
				_contract.location(),
				"Missing '" + p.first + "' with type signature '" + (p.second->toString(false)) + "' " +
					"(if you meant to implement the " + this->name() + " standard)."
			);
		return false;
	default:
		solAssert(false, "Unknown contract standard checker mode.");
	}
}

bool BasicContractStandard::checkFunctionCallExistence
(
	ContractDefinition const& _contract,
	FunctionSignatureHashes const& _requiredFunctionCalls,
	ErrorReporter& _reporter
)
{
	for(pair<string, u256> p: _requiredFunctionCalls)
	{
		bool found = false;
		// m_touch is a callback function that will be called when ast nodes are visited
		m_touch = [&found, &p](ASTNode const* _astNode) {
			if (auto ma = dynamic_cast<MemberAccess const*>(_astNode))
			{
				// is it accessing member function? (trying to cast a shared_ptr<Type const> to a shared_ptr<FunctionType const>)
				if (FunctionTypePointer ftp = dynamic_pointer_cast<FunctionType const>(ma->annotation().type))
				{
					// the member name should equals to the function name, and the hash of function signature should match
					if (ma->memberName() == p.first && ftp->externalIdentifier() == p.second)
						found = true;
				}
			}
		};
		_contract.accept(*this);
		if (!found) {
			_reporter.info(
				_contract.location(),
				"Function call '" + p.first + "' with type signature hash '" + toHex(p.second).substr((256-32)/4) + "' not found in contract " +
					_contract.name() + ". The function call is suggested by " + this->name() + "."
			);
			return false;
		}
	}
	return true;
}

string ERC20ContractStandard::name() const { return "ERC20"; }

FunctionSpecifications const& ERC20ContractStandard::requiredFunctions() const
{
	static FunctionSpecifications ret =
	{	// Note that ERC20 standard says name() / symbol() / decimals() are OPTIONAL
		{"totalSupply", make_shared<FunctionType>(strings(), strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"balanceOf", make_shared<FunctionType>(strings{"address"}, strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"transfer", make_shared<FunctionType>(strings{"address", "uint256"}, strings{"bool"}, FunctionType::Kind::External)},
		{"transferFrom", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings{"bool"}, FunctionType::Kind::External)},
		{"approve", make_shared<FunctionType>(strings{"address", "uint256"}, strings{"bool"}, FunctionType::Kind::External)},
		{"allowance", make_shared<FunctionType>(strings{"address", "address"}, strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"Transfer", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::Event)},
		{"Approval", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::Event)}
	};
	return ret;
}

bool ERC20ContractStandard::checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode)
{
	return checkFunctionExistence(_contract, requiredFunctions(), _reporter, _mode);
}

string ERC223ContractStandard::name() const { return "ERC223"; }

FunctionSpecifications const& ERC223ContractStandard::requiredFunctions() const
{
	static FunctionSpecifications ret =
	{	// we explicitly use "bytes memory" because Type::fromElementaryTypeName(string) defaults to storage,
		// but the default for function parameters in solidity is memory
		{"totalSupply", make_shared<FunctionType>(strings(), strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"balanceOf", make_shared<FunctionType>(strings{"address"}, strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"transfer", make_shared<FunctionType>(strings{"address", "uint256"}, strings{"bool"}, FunctionType::Kind::External)},
		{"transfer", make_shared<FunctionType>(strings{"address", "uint256", "bytes memory"}, strings{"bool"}, FunctionType::Kind::External)},
		{"Transfer", make_shared<FunctionType>(strings{"address", "address", "uint256", "bytes memory"}, strings(), FunctionType::Kind::Event)},
	};
	return ret;
}

FunctionSignatureHashes const& ERC223ContractStandard::requiredFunctionCalls() const
{
	static FunctionSignatureHashes ret =
	{
		{"tokenFallback", 0xC0EE0B8A},
	};
	return ret;
}

bool ERC223ContractStandard::checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode)
{
	return checkFunctionExistence(_contract, requiredFunctions(), _reporter, _mode) &&
			checkFunctionCallExistence(_contract, requiredFunctionCalls(), _reporter);
}

string ERC721ContractStandard::name() const { return "ERC721"; }

FunctionSpecifications const& ERC721ContractStandard::requiredFunctions() const
{
	static FunctionSpecifications ret =
	{	// Function parameters of external functions are forced to calldata
		{"Transfer", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::Event)},
		{"Approval", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::Event)},
		{"ApprovalForAll", make_shared<FunctionType>(strings{"address", "address", "bool"}, strings(), FunctionType::Kind::Event)},
		{"balanceOf", make_shared<FunctionType>(strings{"address"}, strings{"uint256"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"ownerOf", make_shared<FunctionType>(strings{"uint256"}, strings{"address"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"safeTransferFrom", make_shared<FunctionType>(strings{"address", "address", "uint256", "bytes calldata"}, strings(), FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
		{"safeTransferFrom", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
		{"transferFrom", make_shared<FunctionType>(strings{"address", "address", "uint256"}, strings(), FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
		{"approve", make_shared<FunctionType>(strings{"address", "uint256"}, strings(), FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
		{"setApprovalForAll", make_shared<FunctionType>(strings{"address", "bool"}, strings(), FunctionType::Kind::External)},
		{"getApproved", make_shared<FunctionType>(strings{"uint256"}, strings{"address"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"isApprovedForAll", make_shared<FunctionType>(strings{"address", "address"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		{"supportsInterface", make_shared<FunctionType>(strings{"bytes4"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
	};
	return ret;
}

bool ERC721ContractStandard::checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode)
{
	return checkFunctionExistence(_contract, requiredFunctions(), _reporter, _mode);
}

FunctionSpecifications const& ERC827ContractStandard::requiredFunctions() const
{
	static FunctionSpecifications ret = [this]()
	{
		FunctionSpecifications erc827 =
		{
			{"transferAndCall", make_shared<FunctionType>(strings{"address", "uint256", "bytes memory"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
			{"transferFromAndCall", make_shared<FunctionType>(strings{"address", "address", "uint256", "bytes memory"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
			{"approveAndCall", make_shared<FunctionType>(strings{"address", "uint256", "bytes memory"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::Payable)},
		};
		// also need erc20 standard functions
		erc827.insert(erc827.end(), ERC20ContractStandard::requiredFunctions().begin(), ERC20ContractStandard::requiredFunctions().end());
		return erc827;
	}();
	return ret;
}

bool ERC827ContractStandard::checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode)
{
	return checkFunctionExistence(_contract, requiredFunctions(), _reporter, _mode);
}

string ERC827ContractStandard::name() const { return "ERC827"; }

FunctionSpecifications const& ERC884ContractStandard::requiredFunctions() const
{
	static FunctionSpecifications ret = [this]()
	{
		FunctionSpecifications erc884 =
		{
			{"VerifiedAddressAdded", make_shared<FunctionType>(strings{"address", "bytes32", "address"}, strings(), FunctionType::Kind::Event)},
			{"VerifiedAddressRemoved", make_shared<FunctionType>(strings{"address", "address"}, strings(), FunctionType::Kind::Event)},
			{"VerifiedAddressUpdated", make_shared<FunctionType>(strings{"address", "bytes32", "bytes32", "address"}, strings(), FunctionType::Kind::Event)},
			{"VerifiedAddressSuperseded", make_shared<FunctionType>(strings{"address", "address", "address"}, strings(), FunctionType::Kind::Event)},
			{"addVerified", make_shared<FunctionType>(strings{"address", "bytes32"}, strings(), FunctionType::Kind::External)},
			{"removeVerified", make_shared<FunctionType>(strings{"address"}, strings(), FunctionType::Kind::External)},
			{"updateVerified", make_shared<FunctionType>(strings{"address", "bytes32"}, strings(), FunctionType::Kind::External)},
			{"cancelAndReissue", make_shared<FunctionType>(strings{"address", "address"}, strings(), FunctionType::Kind::External)},
			{"isVerified", make_shared<FunctionType>(strings{"address"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"isHolder", make_shared<FunctionType>(strings{"address"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"hasHash", make_shared<FunctionType>(strings{"address", "bytes32"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"holderCount", make_shared<FunctionType>(strings(), strings{"uint"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"holderAt", make_shared<FunctionType>(strings{"uint256"}, strings{"address"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"isSuperseded", make_shared<FunctionType>(strings{"address"}, strings{"bool"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
			{"getCurrentFor", make_shared<FunctionType>(strings{"address"}, strings{"address"}, FunctionType::Kind::External, FunctionType::SpecialModifier::Default, false, StateMutability::View)},
		};
		// also need erc20 standard functions
		erc884.insert(erc884.end(), ERC20ContractStandard::requiredFunctions().begin(), ERC20ContractStandard::requiredFunctions().end());
		return erc884;
	}();
	return ret;
}

string ERC884ContractStandard::name() const { return "ERC884"; }

bool ERC884ContractStandard::checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode)
{
	return checkFunctionExistence(_contract, requiredFunctions(), _reporter, _mode);
}

