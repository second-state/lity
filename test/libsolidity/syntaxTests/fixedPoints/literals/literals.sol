contract C {
    function f() public pure returns (ufixed8x2) {
        /* both are rational 1/10 */
        ufixed8x2 a = 0.1;
        ufixed8x2 b = 0.10;
        return a + b;
    }
    function g() public pure returns (fixed8x1) {
        fixed8x1 a = 0.1;
        return a;
    }
}
