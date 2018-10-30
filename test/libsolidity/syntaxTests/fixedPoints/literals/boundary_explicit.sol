contract C {
    function f() public pure returns (ufixed8x1) {
        ufixed8x1 a = ufixed8x1(25.5);
        return a;
    }
    function g() public pure returns (ufixed8x1) {
        ufixed8x1 a = ufixed8x1(25.6);
        return a;
    }
    function h() public pure returns (fixed8x1) {
        fixed8x1 a = fixed8x1(12.7);
        return a;
    }
    function i() public pure returns (fixed8x1) {
        fixed8x1 a = fixed8x1(12.8);
        return a;
    }
    function j() public pure returns (fixed8x1) {
        fixed8x1 a = fixed8x1(-12.8);
        return a;
    }
    function k() public pure returns (fixed8x1) {
        fixed8x1 a = fixed8x1(-12.9);
        return a;
    }
}
// ----
// TypeError: (200-215): Explicit type conversion not allowed from "rational_const 128 / 5" to "ufixed8x1".
// TypeError: (423-437): Explicit type conversion not allowed from "rational_const 64 / 5" to "fixed8x1".
// TypeError: (646-661): Explicit type conversion not allowed from "rational_const -129 / 10" to "fixed8x1".
