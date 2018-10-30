contract C {
    function f() public pure returns (ufixed8x1) {
        ufixed8x1 a = 25.5;
        return a;
    }
    function g() public pure returns (ufixed8x1) {
        ufixed8x1 a = 25.6;
        return a;
    }
    function h() public pure returns (fixed8x1) {
        fixed8x1 a = 12.7;
        return a;
    }
    function i() public pure returns (fixed8x1) {
        fixed8x1 a = 12.8;
        return a;
    }
    function j() public pure returns (fixed8x1) {
        fixed8x1 a = -12.8;
        return a;
    }
    function k() public pure returns (fixed8x1) {
        fixed8x1 a = -12.9;
        return a;
    }
}
// ----
// TypeError: (175-193): Type rational_const 128 / 5 is not implicitly convertible to expected type ufixed8x1. Try converting to type ufixed16x1 or use an explicit conversion.
// TypeError: (378-395): Type rational_const 64 / 5 is not implicitly convertible to expected type fixed8x1. Try converting to type ufixed8x1 or use an explicit conversion.
// TypeError: (581-599): Type rational_const -129 / 10 is not implicitly convertible to expected type fixed8x1. Try converting to type fixed16x1 or use an explicit conversion.
