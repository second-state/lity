contract C {
    function f() public pure {
      bytes1 b1 = bytes1(0);
      bytes2 b2 = bytes2(0);
      bytes3 b3 = bytes3(0);
      bytes4 b4 = bytes4(0);
      bytes8 b8 = bytes8(0);
      bytes16 b16 = bytes16(0);
      bytes32 b32 = bytes32(0);
      b1; b2; b3; b4; b8; b16; b32;
    }
    function g() public pure {
      bytes1 b1 = bytes1(0x000);
      bytes2 b2 = bytes2(0x00000);
      bytes3 b3 = bytes3(0x0000000);
      bytes4 b4 = bytes4(0x000000000);
      bytes8 b8 = bytes8(0x00000000000000000);
      b1; b2; b3; b4; b8;
    }
    function h() public pure {
      bytes1 b1 = bytes1(0x0);
      bytes2 b2 = bytes2(0x0);
      bytes3 b3 = bytes3(0x0);
      bytes4 b4 = bytes4(0x0);
      bytes8 b8 = bytes8(0x0);
      bytes16 b16 = bytes16(0x0);
      bytes32 b32 = bytes32(0x0);
      b1; b2; b3; b4; b8; b16; b32;
    }
}
// ----
// TypeError: (91-100): Explicit type conversion not allowed from "int_const 0" to "bytes2".
// TypeError: (120-129): Explicit type conversion not allowed from "int_const 0" to "bytes3".
// TypeError: (149-158): Explicit type conversion not allowed from "int_const 0" to "bytes4".
// TypeError: (178-187): Explicit type conversion not allowed from "int_const 0" to "bytes8".
// TypeError: (209-219): Explicit type conversion not allowed from "int_const 0" to "bytes16".
// TypeError: (241-251): Explicit type conversion not allowed from "int_const 0" to "bytes32".
// TypeError: (377-392): Explicit type conversion not allowed from "int_const 0" to "bytes2".
// TypeError: (412-429): Explicit type conversion not allowed from "int_const 0" to "bytes3".
// TypeError: (449-468): Explicit type conversion not allowed from "int_const 0" to "bytes4".
// TypeError: (488-515): Explicit type conversion not allowed from "int_const 0" to "bytes8".
// TypeError: (629-640): Explicit type conversion not allowed from "int_const 0" to "bytes2".
// TypeError: (660-671): Explicit type conversion not allowed from "int_const 0" to "bytes3".
// TypeError: (691-702): Explicit type conversion not allowed from "int_const 0" to "bytes4".
// TypeError: (722-733): Explicit type conversion not allowed from "int_const 0" to "bytes8".
// TypeError: (755-767): Explicit type conversion not allowed from "int_const 0" to "bytes16".
// TypeError: (789-801): Explicit type conversion not allowed from "int_const 0" to "bytes32".
