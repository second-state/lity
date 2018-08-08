pragma solidity ^0.4.0;

contract Smoke {
    uint256[] arr;
    function numberOfElements() public view returns (uint256) {
        return arr.length;
    }
}
