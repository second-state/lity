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
 * Unit tests for ERC827 standard detection
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

class ERC827ContractFramework: public AnalysisFramework
{
protected:
	virtual std::pair<SourceUnit const*, ErrorList>
	parseAnalyseAndReturnError(
		std::string const& _source,
		bool _reportWarnings = false,
		bool _insertVersionPragma = true,
		bool _allowMultipleErrors = true
	) override
	{
		m_compiler.setContractStandard("ERC827");
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(ContractStandardERC827, ERC827ContractFramework)

BOOST_AUTO_TEST_CASE(erc827_normal_contract)
{
	string text = R"(
contract C { function f(uint a, uint b) public pure returns (uint) { return a + b; } }
	)";
	CHECK_INFO(text, "Missing 'transferAndCall'");
}
BOOST_AUTO_TEST_CASE(erc827_standard)
{
	string text = R"(
contract C20 {
  function totalSupply() public view returns (uint256);
  function balanceOf(address who) public view returns (uint256);
  function transfer(address to, uint256 value) public returns (bool);
  function allowance(address owner, address spender) public view returns (uint256);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  event Approval(address indexed owner, address indexed spender, uint256 value);
  event Transfer(address indexed from, address indexed to, uint256 value);
}
contract X is C20 {
  function transferAndCall(address _to, uint256 _value, bytes memory _data) public payable returns (bool);
  function transferFromAndCall( address _from, address _to, uint256 _value, bytes memory _data) public payable returns (bool);
  function approveAndCall(address _spender, uint256 _value, bytes memory _data) public payable returns (bool);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"X is compatible",
		"C20 is not compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc827_standard_no_inheritance)
{
	string text = R"(
contract X {
  function totalSupply() public view returns (uint256);
  function balanceOf(address who) public view returns (uint256);
  function transfer(address to, uint256 value) public returns (bool);
  function allowance(address owner, address spender) public view returns (uint256);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  event Approval(address indexed owner, address indexed spender, uint256 value);
  event Transfer(address indexed from, address indexed to, uint256 value);
  function transferAndCall(address _to, uint256 _value, bytes memory _data) public payable returns (bool);
  function transferFromAndCall( address _from, address _to, uint256 _value, bytes memory _data) public payable returns (bool);
  function approveAndCall(address _spender, uint256 _value, bytes memory _data) public payable returns (bool);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"X is compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc827_standard_wrong_modification_level)
{
	string text = R"(
contract C20 {
  function totalSupply() public view returns (uint256);
  function balanceOf(address who) public view returns (uint256);
  function transfer(address to, uint256 value) public returns (bool);
  function allowance(address owner, address spender) public view returns (uint256);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  event Approval(address indexed owner, address indexed spender, uint256 value);
  event Transfer(address indexed from, address indexed to, uint256 value);
}
contract X is C20 {
  function transferAndCall(address _to, uint256 _value, bytes memory _data) public payable returns (bool);
  function transferFromAndCall( address _from, address _to, uint256 _value, bytes memory _data) public returns (bool); // should be payable
  function approveAndCall(address _spender, uint256 _value, bytes memory _data) public payable returns (bool);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"Missing 'transferAndCall'",
		"Missing 'transferFromAndCall'",
	}));
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
