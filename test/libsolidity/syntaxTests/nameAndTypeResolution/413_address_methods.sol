contract C {
    function f() public {
        address payable addr;
        uint balance = addr.balance;
        (bool callSuc,) = addr.call("");
        (bool delegatecallSuc,) = addr.delegatecall("");
        bool sendRet = addr.send(1);
        addr.transfer(1);
        balance; callSuc; delegatecallSuc; sendRet;
    }
}
// ----
// Warning: (132-141): The use of low level "call" should be avoided. Use direct call to abstract contract instead.
