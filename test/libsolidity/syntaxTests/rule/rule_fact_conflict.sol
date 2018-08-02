contract C {
    struct Person {
        int age;
    }

    rule "myRule" when {
        p: Person(age > 10);
        q: Person(age > 10);
        p: Person(age < 10);
    } then {
    }
}
// ----
// DeclarationError: (148-168): Identifier already declared.
