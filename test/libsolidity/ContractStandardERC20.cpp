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
/**
 * Unit tests for ERC20 standard detection
 */

#include <test/libsolidity/AnalysisFramework.h>

#include <boost/test/unit_test.hpp>

#include <string>

using namespace std;

namespace dev
{
namespace solidity
{
namespace test
{

class ERC20ContractFramework: public AnalysisFramework
{
protected:
	virtual std::pair<SourceUnit const*, langutil::ErrorList>
	parseAnalyseAndReturnError(
		std::string const& _source,
		bool _reportWarnings = false,
		bool _insertVersionPragma = true,
		bool _allowMultipleErrors = true
	) override
	{
		m_compiler.setContractStandard("ERC20");
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(ContractStandardERC20, ERC20ContractFramework)

BOOST_AUTO_TEST_CASE(erc20_normal_contract)
{
	string text = R"(
contract C { function f(uint a, uint b) public pure returns (uint) { return a + b; } }
	)";
	CHECK_INFO(text, "Missing 'totalSupply'");
}
BOOST_AUTO_TEST_CASE(erc20_inheritance)
{
	string text = R"(
contract A { function totalSupply() public view returns (uint); }
contract B is A {
    function balanceOf(address tokenOwner) public view returns (uint balance);
}
contract C {
    event Transfer(address indexed from, address indexed to, uint tokens);
}
contract D is A {
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
contract X is C, B, D {
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);
}
contract Y is X { }
contract Z is C, A, D { } // missing balanceOf(address)
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"A is not compatible",
		"B is not compatible",
		"C is not compatible",
		"D is not compatible",
		"X is compatible",
		"Y is compatible",
		"Z is not compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc20_standard)
{
	string text = R"(
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "X is compatible");
}

BOOST_AUTO_TEST_CASE(erc20_standard_wrong_mutability)
{
	string text = R"(
contract X {
    function totalSupply() public pure returns (uint); // mutability should be view, not pure
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'totalSupply'");
}
BOOST_AUTO_TEST_CASE(erc20_standard_wrong_visibility)
{
	string text = R"(
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) internal view returns (uint balance); // wrong visibility
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'balanceOf'");
}
BOOST_AUTO_TEST_CASE(erc20_standard_missing_event)
{
	string text = R"(
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    // event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'Transfer'");
}
BOOST_AUTO_TEST_CASE(erc20_standard_wrong_method_name)
{
	string text = R"(
contract X {
    function tOtaLSuPpLy() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'totalSupply'");
}
BOOST_AUTO_TEST_CASE(erc20_standard_wrong_type)
{
	string text = R"(
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint128 tokens) public returns (bool success); // uint128 should be uint/uint256
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'transfer'");
}
BOOST_AUTO_TEST_CASE(erc20_fixed_supply_token)
{
	string text = R"(
// 'FIXED' 'Example Fixed Supply Token' token contract
//
// Symbol      : FIXED
// Name        : Example Fixed Supply Token
// Total supply: 1,000,000.000000000000000000
// Decimals    : 18
//
// Enjoy.
//
// (c) BokkyPooBah / Bok Consulting Pty Ltd 2017. The MIT Licence.
// (Modified to avoid irrelevant warnings)
library SafeMath {
    function add(uint a, uint b) internal pure returns (uint c) {
        c = a + b;
        require(c >= a);
    }
    function sub(uint a, uint b) internal pure returns (uint c) {
        require(b <= a);
        c = a - b;
    }
    function mul(uint a, uint b) internal pure returns (uint c) {
        c = a * b;
        require(a == 0 || c / a == b);
    }
    function div(uint a, uint b) internal pure returns (uint c) {
        require(b > 0);
        c = a / b;
    }
}
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
}
contract ApproveAndCallFallBack {
    function receiveApproval(address from, uint256 tokens, address token, bytes memory data) public;
}
contract Owned {
    address public owner;
    address public newOwner;
    event OwnershipTransferred(address indexed _from, address indexed _to);
    constructor() public { owner = msg.sender; }
    modifier onlyOwner { require(msg.sender == owner); _; }
    function transferOwnership(address _newOwner) public onlyOwner { newOwner = _newOwner; }
    function acceptOwnership() public {
        require(msg.sender == newOwner);
        emit OwnershipTransferred(owner, newOwner);
        owner = newOwner;
        newOwner = address(0);
    }
}
contract FixedSupplyToken is X, Owned {
    using SafeMath for uint;
    string public symbol;
    string public  name;
    uint8 public decimals;
    uint public _totalSupply;
    mapping(address => uint) balances;
    mapping(address => mapping(address => uint)) allowed;
    constructor() public {
        symbol = "FIXED";
        name = "Example Fixed Supply Token";
        decimals = 18;
        _totalSupply = 1000000 * 10**uint(decimals);
        balances[owner] = _totalSupply;
        emit Transfer(address(0), owner, _totalSupply);
    }
    function totalSupply() public view returns (uint) { return _totalSupply  - balances[address(0)]; }
    function balanceOf(address tokenOwner) public view returns (uint balance) { return balances[tokenOwner]; }
    function transfer(address to, uint tokens) public returns (bool success) {
        balances[msg.sender] = balances[msg.sender].sub(tokens);
        balances[to] = balances[to].add(tokens);
        emit Transfer(msg.sender, to, tokens);
        return true;
    }
    function approve(address spender, uint tokens) public returns (bool success) {
        allowed[msg.sender][spender] = tokens;
        emit Approval(msg.sender, spender, tokens);
        return true;
    }
    function transferFrom(address from, address to, uint tokens) public returns (bool success) {
        balances[from] = balances[from].sub(tokens);
        allowed[from][msg.sender] = allowed[from][msg.sender].sub(tokens);
        balances[to] = balances[to].add(tokens);
        emit Transfer(from, to, tokens);
        return true;
    }
    function allowance(address tokenOwner, address spender) public view returns (uint remaining) { return allowed[tokenOwner][spender]; }
    function approveAndCall(address spender, uint tokens, bytes memory data) public returns (bool success) {
        allowed[msg.sender][spender] = tokens;
        emit Approval(msg.sender, spender, tokens);
        ApproveAndCallFallBack(spender).receiveApproval(msg.sender, tokens, address(this), data);
        return true;
    }
    function () external payable { revert(); }
    function transferAnyERC20Token(address tokenAddress, uint tokens) public onlyOwner returns (bool success) {
        return X(tokenAddress).transfer(owner, tokens);
    }
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"SafeMath is not compatible",
		"ApproveAndCallFallBack is not compatible",
		"Owned is not compatible",
		"X is compatible",
		"FixedSupplyToken is compatible",
	}));
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
