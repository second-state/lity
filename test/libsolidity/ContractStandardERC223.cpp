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
 * Unit tests for ERC223 standard detection
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

class ERC223ContractFramework: public AnalysisFramework
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
		m_compiler.setContractStandard("ERC223");
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(ContractStandardERC223, ERC223ContractFramework)

BOOST_AUTO_TEST_CASE(erc223_normal_contract)
{
	string text = R"(
contract C { function f(uint a, uint b) public pure returns (uint) { return a + b; } }
	)";
	CHECK_INFO(text, "Missing 'totalSupply'");
}
BOOST_AUTO_TEST_CASE(erc223_inheritance)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint _value, bytes memory _data) public;
}

contract A { function totalSupply() public view returns (uint); }
contract B is A {
    function balanceOf(address tokenOwner) public view returns (uint balance);
}
contract C {
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
contract D is A {
    function transfer(address to, uint tokens) public returns (bool success);
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success) {
		ContractReceiver(to).tokenFallback(msg.sender, tokens, data);
		success = true;
	}
}
contract X is C, B, D { }
contract Y is X { }
contract Z is C, A, D { } // missing balanceOf(address)
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"A is not compatible",
		"B is not compatible",
		"C is not compatible",
		"D is not compatible",
		"X is compatible",
		"Y is compatible",
		"Z is not compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint _value, bytes memory _data) public;
}
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success) {
		ContractReceiver cr = ContractReceiver(to);
		cr.tokenFallback(msg.sender, tokens, "");
		success = true;
	}
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"X is compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_missing_tokenFallback_call)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint _value, bytes memory _data) public;
}
contract X {
	int a;
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address, uint) public returns (bool success) {
		// ContractReceiver cr = ContractReceiver(to);
		// cr.tokenFallback(msg.sender, tokens, "");
		a = 0; // to suppress mutability warning
		success = true;
	}
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"Function call 'tokenFallback' with type signature hash 'c0ee0b8a' not found in contract X",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_external_tokenFallback_call)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint _value, bytes calldata _data) external;
}
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success) {
		ContractReceiver cr = ContractReceiver(to);
		cr.tokenFallback(msg.sender, tokens, "");
		success = true;
	}
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"X is compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_wrong_receiver_function_name)
{
	string text = R"(
contract ContractReceiver {
    function tikenFallback(address _from, uint _value, bytes memory _data) public;
	//       ^ should be tokenFallback
}
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success) {
		ContractReceiver cr = ContractReceiver(to);
		cr.tikenFallback(msg.sender, tokens, "");
		success = true;
	}
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"Function call 'tokenFallback' with type signature hash 'c0ee0b8a' not found in contract X",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_wrong_receiver_function_parameter)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint32 _value, bytes memory _data) public;
	//                                    ^ should be uint
}
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success) {
		ContractReceiver cr = ContractReceiver(to);
		cr.tokenFallback(msg.sender, uint32(tokens), "");
		success = true;
	}
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"Function call 'tokenFallback' with type signature hash 'c0ee0b8a' not found in contract X",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_inherited_receiver_function_call)
{
	string text = R"(
contract ContractReceiver {
    function tokenFallback(address _from, uint _value, bytes memory _data) public;
}
contract C {
	function f(address a) public {
		ContractReceiver cr = ContractReceiver(a);
		cr.tokenFallback(msg.sender, 42, "");
	}
}
contract X is C {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success);
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"ContractReceiver is not compatible",
		"C is not compatible",
		"X is compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc223_standard_missing_transfer_event_data_field)
{
	string text = R"(
contract X {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success);
    function transfer(address to, uint tokens, bytes memory data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens);
}
	)";
	CHECK_INFO(text, "Missing 'Transfer'");
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
