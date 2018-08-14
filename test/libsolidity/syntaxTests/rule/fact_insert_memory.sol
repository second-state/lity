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
// TypeError: (159-200): Unary operator factInsert cannot be applied to type struct C.Person memory
// TypeError: (266-307): Unary operator factInsert cannot be applied to type struct C.Person memory
