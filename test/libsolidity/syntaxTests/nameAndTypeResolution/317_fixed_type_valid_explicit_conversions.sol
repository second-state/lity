contract test {
    function f() public {
        ufixed256x80 a = ufixed256x80(1/3); a;
        ufixed248x80 b = ufixed248x80(1/3); b;
        ufixed8x1 c = ufixed8x1(1/3); c;
    }
}
// ----
// TypeError: (67-84): Explicit type conversion not allowed from "rational_const 1 / 3" to "ufixed256x80".
// TypeError: (114-131): Explicit type conversion not allowed from "rational_const 1 / 3" to "ufixed248x80".
