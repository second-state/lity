contract C {
    function payView() public view {
        fireAllRules;
    }
    function payPure() public pure {
        fireAllRules;
    }
}
// ----
// Warning: (58-70): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
// TypeError: (123-135): Function declared as pure, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
