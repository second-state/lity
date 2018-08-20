================================================================
VM Wrapper
================================================================

.. _VM Wrapper:

VM Wrapper give a way to dynamic load evm shared library which compatible EVMC 5.0.0

Required
--------

- >= EVMC 5.0


Example
-------

Use Hera VM for example.
First, clone it from github and build with `-DBUILD_SHARED_LIBS=ON`.

To demo the result, we can modify `hera.cpp` to show some information. Modified hera will show

- EVM binary code in hex.
- OP TIMESTAMP result.

.. code::

  diff --git a/src/hera.cpp b/src/hera.cpp
  index 6a7d742..fd60a93 100644
  --- a/src/hera.cpp
  +++ b/src/hera.cpp
  @@ -363,13 +363,24 @@ evmc_result hera_execute(
     memset(&ret, 0, sizeof(evmc_result));

     try {
  -    heraAssert(rev == EVMC_BYZANTIUM, "Only Byzantium supported.");
  +    heraAssert(rev != EVMC_BYZANTIUM, "Only Byzantium supported.");
       heraAssert(msg->gas >= 0, "Negative startgas?");

       bool meterInterfaceGas = true;

       // the bytecode residing in the state - this will be used by interface methods (i.e. codecopy)
       vector<uint8_t> state_code(code, code + code_size);
  +
  +    cout<<" Show Byte Code:"<<endl;
  +    cout<<"   = size:"<<code_size<<endl;
  +    for(int v:state_code)
  +      cout<<hex<<v<<' ';
  +    cout<<endl;
  +
  +    evmc_tx_context tx_context;
  +    context->fn_table->get_tx_context(&tx_context, context);
  +    time_t timestamp = tx_context.block_timestamp;
  +    cout<< "TIMESTAMP: "<< put_time(localtime(&timestamp), "%c %Z") <<endl;

       // the actual executable code - this can be modified (metered or evm2wasm compiled)
       vector<uint8_t> run_code(code, code + code_size);

Build Hera and copy src\libhera.so to where you want to.

.. code::

  git clone https://github.com/ewasm/hera.git --recuresive
  cd hera
  cp ../modified_hera.cpp ./src/hera.cpp
  mkdir build
  cd build
  cmake .. -DBUILD_SHARED_LIBS=ON
  make


Use EVM Wrapper
---------------

Go wrapper will check environment variable `EVMC_PATH` and load VM by this variable.

.. code::

  echo 64acacacacac7f7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0c > tmp.bin

  export EVMC_PATH="./libhera.so"
  ./evm --codefile tmp.bin --debug run

Evm will show

.. code::

  Show Byte Code:
   = size:40
  64 ac ac ac ac ac 7f 7f ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff c
  TIMESTAMP: Sun Aug  5 01:41:31 2018 DST
  0x
  error: evmc: failure
  #### TRACE ####
  #### LOGS ####
