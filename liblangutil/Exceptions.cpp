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
 * @author Liana <liana@ethdev.com>
 * @date 2015
 * Solidity exception hierarchy.
 */

#include <liblangutil/Exceptions.h>

using namespace std;
using namespace dev;
using namespace langutil;

Error::Error(Type _type, SourceLocation const& _location, string const& _description):
	m_type(_type)
{
	m_typeName = typeToCStr(m_type);

	if (!_location.isEmpty())
		*this << errinfo_sourceLocation(_location);
	if (!_description.empty())
		*this << errinfo_comment(_description);
}

Error::Error(Error::Type _type, const std::string& _description, const SourceLocation& _location):
	Error(_type)
{
	if (!_location.isEmpty())
		*this << errinfo_sourceLocation(_location);
	*this << errinfo_comment(_description);
}

char const* Error::typeToCStr(Type _type)
{
	switch (_type)
	{
	case Type::DeclarationError:
		return "DeclarationError";
	case Type::DocstringParsingError:
		return "DocstringParsingError";
	case Type::ParserError:
		return "ParserError";
	case Type::SyntaxError:
		return "SyntaxError";
	case Type::TypeError:
		return "TypeError";
	case Type::Warning:
		return "Warning";
	case Type::Info:
		return "Info";
	default:
		return "";
	}
}
