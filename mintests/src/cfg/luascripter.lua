-- =============================================================================
-- GLOBAL VARIABLES
-- =============================================================================

MOD="/usr/lib/min-tests/LuaTestClass.so";
MOD2="/usr/lib/min-tests/HardCoded.so";

-- =============================================================================
-- TEST CASES
-- =============================================================================

--------------------------------------------------------------------------------

function case_test1() -- Createx test case
        local a = min.load(MOD);
        local ret = a.Example();
        return ret,"Passed";
end

--------------------------------------------------------------------------------

function case_test2() -- Passed Test Case
        return TP_PASSED,"Test Case is Passed";
end

--------------------------------------------------------------------------------

function case_test11() -- Run testmodule by id cfg file specified
        ret = min.run(MOD2,"dummy.cfg",2);
        return TP_PASSED;
end

--------------------------------------------------------------------------------

function case_test12() -- Run testmodule by id no cfg file
        ret = min.run(MOD2,2);
        return TP_PASSED;
end

--------------------------------------------------------------------------------

function case_test13() -- Run testmodule by name cfg file specified
        ret = min.run(MOD2,"dummy.cfg","Test Case 2");
        return TP_PASSED;
end

--------------------------------------------------------------------------------

function case_test14() -- Run testmodule by name no cfg file
        ret = min.run(MOD2,"Test Case 2");
        return TP_PASSED;
end

--------------------------------------------------------------------------------

