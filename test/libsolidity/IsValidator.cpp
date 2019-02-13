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

class IsValidatorFramework: public AnalysisFramework
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
		m_compiler.setEVMVersion(EVMVersion::lity());
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(IsValidator, IsValidatorFramework)

BOOST_AUTO_TEST_CASE(isvalidator_normal)
{
	string text = R"(
contract A { function f(address addr) public pure returns (bool) { return isValidator(addr); } }
	)";
	CHECK_SUCCESS(text);
}
BOOST_AUTO_TEST_CASE(isvalidator_type_error)
{
	string text = R"(
contract A { function f(int addr) public pure returns (bool) { return isValidator(addr); } }
	)";
	CHECK_ERROR(text, TypeError, "Invalid type for argument in function call. Invalid implicit conversion from int256 to address requested.");
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
