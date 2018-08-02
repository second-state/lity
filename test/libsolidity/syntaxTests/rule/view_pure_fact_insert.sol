contract C {
    struct Person {
        int age;
        bool eligible;
        address addr;
    }
    function addPersonView(int age) public view {
        factInsert Person(age, false, address(0));
    }
    function addPersonPure(int age) public pure {
        factInsert Person(age, false, address(0));
    }
}
// ----
// Warning: (159-200): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
// TypeError: (266-307): Function declared as pure, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
